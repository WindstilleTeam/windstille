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

#ifndef HEADER_WINDSTILLE_COLLISION_CONTACT_HPP
#define HEADER_WINDSTILLE_COLLISION_CONTACT_HPP

#include "math/vector2f.hpp"

/** A Contact is a helper class that allows the player to either
    attach to other objects or move along them */
class Contact
{
private:
public:
  Contact()  {}
  virtual ~Contact() {}

  virtual void update(float delta) =0;
  virtual void advance(float s) =0;
  virtual bool is_active() const =0;
  virtual Vector2f get_pos() const =0;
private:
  Contact (const Contact&);
  Contact& operator= (const Contact&);
};

#endif

/* EOF */
