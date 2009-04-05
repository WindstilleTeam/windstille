/*
**  Windstille - A Sci-Fi Action-Adventure Game
**  Copyright (C) 2009 Ingo Ruhnke <grumbel@gmx.de>
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

#include <iostream>
#include <stdexcept>
#include <memory>

#include "math/point.hpp"
#include "display/software_surface.hpp"

#include "texture_packer.hpp"

class TextureSpace
{
private:
  Rect          rect;
  bool          used;
  std::auto_ptr<TextureSpace> left;
  std::auto_ptr<TextureSpace> right;

public:
  TextureSpace(const Rect& rect_)
    : rect(rect_),
      used(false)
  {}

  ~TextureSpace()
  {}

  bool allocate(const Size& size, Rect& out_rect)
  {
    if (size.width  <= rect.get_width() &&
        size.height <= rect.get_height())
      {
        if (!used)
          {
            used = true;
            out_rect = Rect(Point(rect.left, rect.top), size);

            // FIXME: Make this alterate between horizontal and vertical splitting
            left.reset(new TextureSpace(Rect(out_rect.left,  out_rect.bottom,
                                             out_rect.right, rect.bottom)));

            right.reset(new TextureSpace(Rect(out_rect.right, out_rect.top, 
                                              rect.right, rect.bottom)));

            return true;
          }
        else
          {
            return
              left->allocate(size, out_rect) ||
              right->allocate(size, out_rect);
          }
      }
    else
      {
        // Size doesn't fit here
        return false;
      }
  }

private:
  TextureSpace(const TextureSpace&);
  TextureSpace& operator=(const TextureSpace&);
};

class TexturePackerTexture
{
private:
  Texture        texture;
  TextureSpace   space;

public:
  TexturePackerTexture(const Size& size)
    : texture(GL_TEXTURE_2D, size.width, size.height),
      space(Rect(Point(0, 0), size))
  {
  }

  ~TexturePackerTexture()
  {}

  bool allocate(const Size& size, Rect& out_rect, Texture& out_texture)
  {
    if (space.allocate(size, out_rect))
      {
        out_texture = texture;
        return true;
      }
    else
      {
        return false;
      }
  }

private:
  TexturePackerTexture(const TexturePackerTexture&);
  TexturePackerTexture& operator=(const TexturePackerTexture&);
};

TexturePacker::TexturePacker(const Size& texture_size_)
  : texture_size(texture_size_)
{
}

TexturePacker::~TexturePacker()
{
  for(Textures::iterator i = textures.begin(); i != textures.end(); ++i)
    delete *i;
  textures.clear();
}
  
bool
TexturePacker::allocate(const Size& size, Rect& rect, Texture& texture)
{
  for(Textures::iterator i = textures.begin(); i != textures.end(); ++i)
    {
      if ((*i)->allocate(size, rect, texture))
        {
          return true;
        }
    }

  textures.push_back(new TexturePackerTexture(texture_size));
  return textures.back()->allocate(size, rect, texture);
}

Surface
TexturePacker::upload(const SoftwareSurface& in_surface)
{
  // Add a 1px border around surfaces to avoid blending artifacts
  SoftwareSurface surface = in_surface.add_1px_border();

  Size    size(surface.get_width(), surface.get_height());
  Rect    rect;
  Texture texture;

  if (!allocate(size, rect, texture))
    {
      throw std::runtime_error("TexturePacker::upload: texture space allocation failed");
    }
  else
    {
      texture.put(surface, rect.left, rect.top);

      //std::cout << "Texture: " << texture.get_handle() << " " << rect << std::endl;

      return Surface(texture,
                     Rectf(float(rect.left+1)  / texture.get_width(), float(rect.top+1)    / texture.get_height(),
                           float(rect.right-1) / texture.get_width(), float(rect.bottom-1) / texture.get_height()),
                     in_surface.get_width(), in_surface.get_height());
    }
}

/* EOF */
