/*
**  Windstille - A Sci-Fi Action-Adventure Game
**  Copyright (C) 2005 Ingo Ruhnke <grumbel@gmx.de>
**
**  This program is free software: you can redistribute it and/or modify
**  it under the terms of the GNU General Public License as published by
**  the Free Software Foundation, either version 3 of the License, or
**  (at your option) any later version.
**  
**  This program is distributed in the hope that it will be useful,
**  but WITHOUT ANY WARRANTY; without even the implied warranty of
**  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
**  GNU General Public License for more details.
**  
**  You should have received a copy of the GNU General Public License
**  along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <assert.h>
#include <vector>
#include <stdexcept>
#include <sstream>

#include <ft2build.h>
#include FT_FREETYPE_H
#include FT_GLYPH_H

#include "display/opengl_state.hpp"
#include "physfs/physfs_stream.hpp"
#include "display/texture_manager.hpp"
#include "display/blitter.hpp"
#include "ttf_font.hpp"
#include "font_effect.hpp"
#include "util/util.hpp"

TTFCharacter::TTFCharacter(const Rect& pos_,
                           const Rectf& uv_, 
                           int advance_)
  : pos(pos_),
    uv(uv_), 
    advance(advance_)
{
}

class TTFFontImpl
{
public:
  /// the global FreeType library handle
  static FT_Library      library;

  /** Array of characters available in this font, current limited to
      256 characters, no full unicode */
  std::vector<TTFCharacter> characters;

  /** The original size of the font in pixels */
  int size;

  /** OpenGL Texture which holds all the characters */
  Texture texture;
};

FT_Library TTFFontImpl::library;

TTFFont::TTFFont(const std::string& filename, int size_, const FontEffect& effect)
  : impl(new TTFFontImpl())
{
  assert(size_ > 0);

  impl->size = size_;

  IFileStream fin(filename);
  std::istreambuf_iterator<char> first(fin), last;
  std::vector<char> buffer(first, last); 

  FT_Face face;
  if (FT_New_Memory_Face(TTFFontImpl::library, 
                         reinterpret_cast<FT_Byte*>(&*buffer.begin()), buffer.size(), 
                         0, &face))
    {
      throw std::runtime_error("Couldn't load font: '" + filename + "'");
    }
  
  FT_Set_Pixel_Sizes(face, impl->size, impl->size);

  FT_Select_Charmap(face,  FT_ENCODING_UNICODE);

  // FIXME: should calculate texture size, based on font size
  SDL_Surface* pixelbuffer = create_surface_rgba(1024, 1024);
  SDL_SetAlpha(pixelbuffer, 0, 0);

  int x_pos = 1;
  int y_pos = 1;
  
  // FIXME: Not really needed, instead we should sort the characters
  // after glyph_height before rendering and then use the max_glyph_height for each row
  int max_glyph_height = effect.get_height(impl->size); 

  // List of characters we want in the final font
  std::vector<uint32_t> characters;
  for(int i = 0; i < 256; i += 1)
    characters.push_back(i);

  // FIXME: Insert code to sort characters after glyph height
  // FIXME: Insert code that calculates the needed texture size

  // We limit ourself to 256 characters for the momemnt
  for(std::vector<uint32_t>::iterator i = characters.begin(); i != characters.end(); ++i)
    {
      if (FT_Load_Char( face,  *i, FT_LOAD_RENDER))
        {
          // FIXME: What happens when character is not in font, should be handled more gentle
          throw std::runtime_error("couldn't load char");
        }
      else
        {      
          effect.blit(pixelbuffer, face->glyph->bitmap, x_pos, y_pos);

          int glyph_width  = effect.get_glyph_width(face->glyph->bitmap.width);
          int glyph_height = effect.get_glyph_height(face->glyph->bitmap.rows);

          generate_border(pixelbuffer, x_pos, y_pos, glyph_width, glyph_height);

          Rect pos(Point(effect.get_x_offset(face->glyph->bitmap_left),
                         effect.get_y_offset(-face->glyph->bitmap_top)),
                   Size(glyph_width, glyph_height));

          Rectf uv(x_pos/float(pixelbuffer->w),
                   y_pos/float(pixelbuffer->h),
                   (x_pos + glyph_width)/float(pixelbuffer->w),
                   (y_pos + glyph_height)/float(pixelbuffer->h));
      
          impl->characters.push_back(TTFCharacter(pos, uv,
                                                  face->glyph->advance.x >> 6));

          // we leave a one pixel border around the letters which we fill with generate_border
          x_pos += glyph_width + 2;
          if (x_pos + max_glyph_height + 2 > pixelbuffer->w) // FIXME: should use glyph_width of the next glyph instead of max_glyph_height
            {
              y_pos += max_glyph_height + 2;
              x_pos = 1;
            }

          if (y_pos + max_glyph_height + 2 > pixelbuffer->h)
            throw std::runtime_error("Font Texture to small");
        }
    }
  FT_Done_Face(face);

  try 
    {
      impl->texture = Texture(pixelbuffer);
      impl->texture.set_filter(GL_NEAREST);
    }
  catch(...) 
    {
      SDL_FreeSurface(pixelbuffer);
      throw;
    }
  SDL_FreeSurface(pixelbuffer);
}

TTFFont::~TTFFont()
{
  delete impl;
}

const TTFCharacter&
TTFFont::get_character(int c) const
{
  assert(c >= 0 && c < 256);
  return impl->characters[c];
}

int
TTFFont::get_height() const
{
  return impl->size;
}

void
TTFFont::draw(const Vector2f& pos_, const std::string& str, const Color& color)
{
  // FIXME: Little bit hacky to throw it just in
  Vector2f pos(static_cast<int>(pos_.x),
             static_cast<int>(pos_.y));

  OpenGLState state;

  state.enable(GL_BLEND);
  state.set_blend_func(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  state.bind_texture(impl->texture);
  state.color(color);
  state.activate();

  // Voodoo to get non-blurry fonts
  float mx = -0.375;
  float my = -0.375; 
  glBegin(GL_QUADS);
  for(std::string::const_iterator i = str.begin(); i != str.end(); ++i)
    {
      const TTFCharacter& character = impl->characters[*i];
      
      glTexCoord2f(character.uv.left, character.uv.top);
      glVertex2f(pos.x + character.pos.left + mx,
                 pos.y + character.pos.top  + my);

      glTexCoord2f(character.uv.right, character.uv.top);
      glVertex2f(pos.x + character.pos.right + mx, 
                 pos.y + character.pos.top   + my);

      glTexCoord2f(character.uv.right, character.uv.bottom);
      glVertex2f(pos.x + character.pos.right  + mx, 
                 pos.y + character.pos.bottom + my);

      glTexCoord2f(character.uv.left, character.uv.bottom);
      glVertex2f(pos.x + character.pos.left   + mx, 
                 pos.y + character.pos.bottom + my);

      pos.x += character.advance;       
    }
  glEnd();
}

void
TTFFont::draw_center(const Vector2f& pos, const std::string& str, const Color& color)
{
  draw(Vector2f(pos.x - get_width(str)/2, pos.y), str, color);
}

int
TTFFont::get_width(const std::string& text) const
{
  int width = 0;
  for(std::string::const_iterator i = text.begin(); i != text.end(); ++i)
    width += impl->characters[*i].advance;
  return width;
}

Texture
TTFFont::get_texture() const
{
  return impl->texture;
}

void
TTFFont::init()
{
  FT_Error  error;
  
  error = FT_Init_FreeType( &TTFFontImpl::library );
  if ( error )
    throw std::runtime_error( "could not initialize FreeType" );
}

void
TTFFont::deinit()
{
  FT_Done_FreeType( TTFFontImpl::library );
}

/* EOF */
