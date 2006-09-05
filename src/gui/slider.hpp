/*  $Id$
**   __      __ __             ___        __   __ __   __
**  /  \    /  \__| ____    __| _/_______/  |_|__|  | |  |   ____
**  \   \/\/   /  |/    \  / __ |/  ___/\   __\  |  | |  | _/ __ \
**   \        /|  |   |  \/ /_/ |\___ \  |  | |  |  |_|  |_\  ___/
**    \__/\  / |__|___|  /\____ /____  > |__| |__|____/____/\___  >
**         \/          \/      \/    \/                         \/
**  Copyright (C) 2005 Ingo Ruhnke <grumbel@gmx.de>
**
**  This program is free software; you can redistribute it and/or
**  modify it under the terms of the GNU General Public License
**  as published by the Free Software Foundation; either version 2
**  of the License, or (at your option) any later version.
**
**  This program is distributed in the hope that it will be useful,
**  but WITHOUT ANY WARRANTY; without even the implied warranty of
**  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
**  GNU General Public License for more details.
** 
**  You should have received a copy of the GNU General Public License
**  along with this program; if not, write to the Free Software
**  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA
**  02111-1307, USA.
*/

#ifndef HEADER_SLIDER_HPP
#define HEADER_SLIDER_HPP

#include "component.hpp"

namespace GUI {

/** */
class Slider : public Component
{
private:
  int min;
  int max;
  int step;
  int pos;
  bool horizontal;

public:
  Slider(Component* parent);
  ~Slider();

  int  get_pos() const;

  void set_pos(int pos);
  int  set_minimum(int min);
  int  set_maximum(int max);
  void set_range(int min, int max);
  void set_step(int step);

  void draw();
  void update(float delta, const Controller& controller);

private:
  Slider (const Slider&);
  Slider& operator= (const Slider&);
};

} // namespace GUI

#endif

/* EOF */
