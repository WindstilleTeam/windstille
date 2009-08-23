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
#include <boost/scoped_ptr.hpp>

#include "math/point.hpp"
#include "display/software_surface.hpp"

#include "texture_packer.hpp"

class TextureSpace
{
private:
  Rect          rect;
  bool          used;
  boost::scoped_ptr<TextureSpace> left;
  boost::scoped_ptr<TextureSpace> right;

public:
  TextureSpace(const Rect& rect_)
    : rect(rect_),
      used(false),
      left(),
      right()
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

            // FIXME: Make this alterate between horizontal and
            // vertical splitting or chose whichever split options
            // leads to less 'ugly' rectangle (how much different does
            // this make in terms of packing density?)
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

  Texture get_texture() const { return texture; }

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
TexturePacker::upload(const SoftwareSurface& surface)
{
  // Add a 1px border around surfaces to avoid blending artifacts
  //SoftwareSurface surface = in_surface.add_1px_border();

  Size    size(surface.get_width()+2, surface.get_height()+2);
  Rect    rect;
  Texture texture;

  if (!allocate(size, rect, texture))
    {
      throw std::runtime_error("TexturePacker::upload: texture space allocation failed");
    }
  else
    {
      // duplicate border pixel

      // top
      texture.put(surface, Rect(Point(0, 0), Size(surface.get_width(), 1)), 
                  rect.left+1, rect.top);
      // bottom
      texture.put(surface, Rect(Point(0, surface.get_height()-1), Size(surface.get_width(), 1)), 
                  rect.left+1, rect.bottom-1);
      // left
      texture.put(surface, Rect(Point(0, 0), Size(1, surface.get_height())), 
                  rect.left, rect.top+1);
      // right
      texture.put(surface, Rect(Point(surface.get_width()-1, 0), Size(1, surface.get_height())),
                  rect.right-1, rect.top+1);

      // duplicate corner pixels
      texture.put(surface, Rect(Point(0, 0), Size(1, 1)), 
                  rect.left, rect.top);     
      texture.put(surface, Rect(Point(surface.get_width()-1, 0), Size(1, 1)), 
                  rect.right-1, rect.top);
      texture.put(surface, Rect(Point(surface.get_width()-1, surface.get_height()-1), Size(1, 1)), 
                  rect.right-1, rect.bottom-1);
      texture.put(surface, Rect(Point(0, surface.get_height()-1), Size(1, 1)),
                  rect.left, rect.bottom-1);

      // draw the main surface
      texture.put(surface, rect.left+1, rect.top+1);

      return Surface(texture,
                     Rectf(float(rect.left+1)  / texture.get_width(), float(rect.top+1)    / texture.get_height(),
                           float(rect.right-1) / texture.get_width(), float(rect.bottom-1) / texture.get_height()),
                     Sizef(surface.get_width(), surface.get_height()));
    }
}

void
TexturePacker::save_all_as_png() const
{
  for(Textures::const_iterator i = textures.begin(); i != textures.end(); ++i)
    {
      Texture texture = (*i)->get_texture();
      SoftwareSurface surface = texture.get_software_surface();

      char filename[1024];
      sprintf(filename, "/tmp/texture_packer%04d.png", int(i - textures.begin()));
      std::cout << "Saving: " << filename << std::endl;
      surface.save_png(filename);
    }
}

/* EOF */
