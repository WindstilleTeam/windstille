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

#ifndef HEADER_WINDSTILLE_GUI_SLIDER_HPP
#define HEADER_WINDSTILLE_GUI_SLIDER_HPP

#include "gui/component.hpp"

namespace gui {

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

} // namespace gui

#endif

/* EOF */
