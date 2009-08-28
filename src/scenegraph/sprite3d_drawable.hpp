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
  const Sprite3D* sprite;

public:
  Sprite3DDrawable(const Sprite3D* sprite_, 
                         const Vector2f& pos_, float z_pos_, const Matrix& modelview_)
    : Drawable(pos_, z_pos_, modelview_), sprite(sprite_)
  {}

  void draw(const Texture& /*tmp_texture*/)
  {
    sprite->draw(pos, modelview);
  }

  void set_pos(const Vector2f& pos_)
  {
    pos = pos_;
  }

private:
  Sprite3DDrawable(const Sprite3DDrawable&);
  Sprite3DDrawable& operator=(const Sprite3DDrawable&);
};

#endif

/* EOF */
