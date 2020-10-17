/*
**  Windstille - A Sci-Fi Action-Adventure Game
**  Copyright (C) 2005 Ingo Ruhnke <grumbel@gmail.com>
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

#include <stdint.h>
#include <vector>
#include <stdexcept>
#include <sstream>
#include <fstream>

#include <ft2build.h>
#include <geom/geom.hpp>

#include "display/blitter.hpp"
#include "display/drawing_context.hpp"
#include "display/software_surface.hpp"
#include "font/text_drawable.hpp"
#include "font/ttf_font.hpp"
#include "scenegraph/vertex_array_drawable.hpp"

TTFCharacter::TTFCharacter(const geom::irect& pos_,
                           const geom::frect& uv_,
                           int advance_) :
  pos(pos_),
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
  TexturePtr texture;

  TTFFontImpl() :
    characters(),
    size(0),
    texture()
  {}
};

FT_Library TTFFontImpl::library;

TTFFont::TTFFont(std::filesystem::path const& filename, int size_, const FontEffect& effect) :
  impl(new TTFFontImpl())
{
  assert(size_ > 0);

  impl->size = size_;

  std::ifstream fin(filename, std::ios::binary);
  std::istreambuf_iterator<char> first(fin), last;
  std::vector<char> buffer(first, last);

  FT_Face face;
  if (FT_New_Memory_Face(TTFFontImpl::library,
                         reinterpret_cast<FT_Byte*>(&*buffer.begin()), buffer.size(),
                         0, &face))
  {
    std::ostringstream str;
    str << "Couldn't load font: " << filename;
    throw std::runtime_error(str.str());
  }

  FT_Set_Pixel_Sizes(face, impl->size, impl->size);

  FT_Select_Charmap(face,  FT_ENCODING_UNICODE);

  // FIXME: should calculate texture size, based on font size
  SoftwareSurfacePtr pixelbuffer = SoftwareSurface::create(SoftwareSurface::RGBA, 1024, 1024);

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

      int glyph_width  = effect.get_glyph_width(static_cast<int>(face->glyph->bitmap.width));
      int glyph_height = effect.get_glyph_height(static_cast<int>(face->glyph->bitmap.rows));

      generate_border(pixelbuffer, x_pos, y_pos, glyph_width, glyph_height);

      geom::irect pos(geom::ipoint(effect.get_x_offset(face->glyph->bitmap_left),
                     effect.get_y_offset(-face->glyph->bitmap_top)),
               geom::isize(glyph_width, glyph_height));

      geom::frect uv(static_cast<float>(x_pos) / static_cast<float>(pixelbuffer->get_width()),
               static_cast<float>(y_pos) / static_cast<float>(pixelbuffer->get_height()),
               static_cast<float>(x_pos + glyph_width)/static_cast<float>(pixelbuffer->get_width()),
               static_cast<float>(y_pos + glyph_height)/static_cast<float>(pixelbuffer->get_height()));

      impl->characters.push_back(TTFCharacter(pos, uv,
                                              static_cast<int>(face->glyph->advance.x >> 6)));

      // we leave a one pixel border around the letters which we fill with generate_border
      x_pos += glyph_width + 2;
      if (x_pos + max_glyph_height + 2 > pixelbuffer->get_width()) // FIXME: should use glyph_width of the next glyph instead of max_glyph_height
      {
        y_pos += max_glyph_height + 2;
        x_pos = 1;
      }

      if (y_pos + max_glyph_height + 2 > pixelbuffer->get_height())
        throw std::runtime_error("Font Texture to small");
    }
  }
  FT_Done_Face(face);

  impl->texture = Texture::create(pixelbuffer);
}

TTFFont::~TTFFont()
{
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
TTFFont::draw(GraphicsContext& gc, const glm::vec2& pos_, const std::string& str, const Color& color)
{
  // FIXME: Little bit hacky to throw it just in
  glm::vec2 pos(truncf(pos_.x),
                truncf(pos_.y));

  VertexArrayDrawable va;

  va.set_blend_func(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  va.set_texture(impl->texture);

  va.set_mode(GL_TRIANGLES);
  for(std::string::const_iterator i = str.begin(); i != str.end(); ++i)
  {
    const TTFCharacter& character = impl->characters[*i];

    // v1
    va.color(color);
    va.texcoord(character.uv.left(), character.uv.top());
    va.vertex(pos.x + static_cast<float>(character.pos.left()),
              pos.y + static_cast<float>(character.pos.top()));
    // v4
    va.color(color);
    va.texcoord(character.uv.left(), character.uv.bottom());
    va.vertex(pos.x + static_cast<float>(character.pos.left()),
              pos.y + static_cast<float>(character.pos.bottom()));
    // v2
    va.color(color);
    va.texcoord(character.uv.right(), character.uv.top());
    va.vertex(pos.x + static_cast<float>(character.pos.right()),
              pos.y + static_cast<float>(character.pos.top()));

    // v4
    va.color(color);
    va.texcoord(character.uv.left(), character.uv.bottom());
    va.vertex(pos.x + static_cast<float>(character.pos.left()),
              pos.y + static_cast<float>(character.pos.bottom()));
    // v3
    va.color(color);
    va.texcoord(character.uv.right(), character.uv.bottom());
    va.vertex(pos.x + static_cast<float>(character.pos.right()),
              pos.y + static_cast<float>(character.pos.bottom()));
    // v2
    va.color(color);
    va.texcoord(character.uv.right(), character.uv.top());
    va.vertex(pos.x + static_cast<float>(character.pos.right()),
              pos.y + static_cast<float>(character.pos.top()));

    pos.x += static_cast<float>(character.advance);
  }
  va.render(gc);
}

void
TTFFont::draw_center(GraphicsContext& gc, const glm::vec2& pos, const std::string& str, const Color& color)
{
  draw(gc, glm::vec2(pos.x - static_cast<float>(get_width(str))/2.0f, pos.y), str, color);
}

void
TTFFont::draw(DrawingContext& ctx, const std::string& text, float x, float y, float z)
{
  ctx.draw(std::make_unique<TextDrawable>(*this, text, glm::vec2(x, y), z, ctx.get_modelview()));
}

int
TTFFont::get_width(const std::string& text) const
{
  int width = 0;
  for(std::string::const_iterator i = text.begin(); i != text.end(); ++i)
    width += impl->characters[*i].advance;
  return width;
}

TexturePtr
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

TTFFontManager::TTFFontManager()
{
  TTFFont::init();
}

TTFFontManager::~TTFFontManager()
{
  TTFFont::deinit();
}

/* EOF */
