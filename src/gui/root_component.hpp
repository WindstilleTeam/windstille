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

#ifndef HEADER_WINDSTILLE_GUI_ROOT_COMPONENT_HPP
#define HEADER_WINDSTILLE_GUI_ROOT_COMPONENT_HPP

#include <vector>

#include "gui/component.hpp"

namespace gui {

/** */
class RootComponent : public Component
{
private:
  Component* focus;

  typedef std::vector<Component*> Children;
  Children children;

public:
  RootComponent(const Rectf& rect);
  ~RootComponent();

  void draw();
  void update(float delta, const Controller& controller);

  /** Set the chidren that shall recieve input */
  void set_focus(Component* child);

  void add_child(Component* child);

  Component* get_focus() const { return focus; }
  
  bool is_active() const;
private:
  RootComponent (const RootComponent&);
  RootComponent& operator= (const RootComponent&);
};

} // namespace gui

#endif

/* EOF */
