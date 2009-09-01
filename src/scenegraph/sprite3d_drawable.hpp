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

#ifndef HEADER_WINDSTILLE_SCENEGRAPH_SPRITE3D_DRAWABLE_HPP
#define HEADER_WINDSTILLE_SCENEGRAPH_SPRITE3D_DRAWABLE_HPP

#include "sprite3d/sprite3d.hpp"
#include "scenegraph/drawable.hpp"

class Sprite3DDrawable : public Drawable
{
private:
  Sprite3D m_sprite;
  float    m_scale;

public:
  Sprite3DDrawable(const Sprite3D& sprite, 
                   const Vector2f& pos_, float z_pos_, const Matrix& modelview_)
    : Drawable(pos_, z_pos_, modelview_), 
      m_sprite(sprite),
      m_scale(1.0f)
  {}

  void draw()
  {
    Matrix matrix = modelview;
    matrix = matrix.translate(pos.x, pos.y, z_pos);
    matrix = matrix.scale(m_scale, m_scale, m_scale);
    m_sprite.draw(Vector2f(0.0f, 0.0f), matrix);
  }

  void set_pos(const Vector2f& pos_)
  {
    pos = pos_;
  }

  float get_scale() const
  { 
    return m_scale;
  }

  void set_scale(float scale) 
  { 
    m_scale = scale;
  }

  Sprite3D& get_sprite()
  { 
    return m_sprite; 
  }

private:
  Sprite3DDrawable(const Sprite3DDrawable&);
  Sprite3DDrawable& operator=(const Sprite3DDrawable&);
};

#endif

/* EOF */
