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

#ifndef HEADER_WINDSTILLE_GUI_GRID_COMPONENT_HPP
#define HEADER_WINDSTILLE_GUI_GRID_COMPONENT_HPP

#include "lisp/lisp.hpp"
#include "field.hpp"
#include "component.hpp"

namespace GUI {

/** */
class GridComponent : public Component
{
private:
  struct ComponentBox 
  {
    enum { } type;

    Component* component;
    Size  span;
    Point parent;

    ComponentBox() 
      : component(0), span(1, 1), parent(-1, -1) {}

    ComponentBox(Component* c, const Size& s, const Point& parent_ = Point(-1, -1))
      : component(c), span(s), parent(parent_) {}

    bool is_simple()  const { return (span.width == 1 && span.height == 1); }
    bool has_parent() const { return (parent.x != -1 || parent.y != -1); }
  };

  typedef Field<ComponentBox> Grid;
  Grid  grid;
  Point pos;
  bool  child_active;
  float padding;

public:
  GridComponent(const lisp::Lisp* lisp, Component* parent);
  GridComponent(const Rectf& rect, int weight, int height, Component* parent);
  ~GridComponent();
  
  void draw();
  void update(float delta, const Controller& controller);

  void pack(Component* component, int x, int y, int colspan = 1, int rowspan = 1);

  void move_up();
  void move_down();

  void move_left();
  void move_right();

  void on_activation();

  void set_padding(float p);
  
private:
  GridComponent (const GridComponent&);
  GridComponent& operator= (const GridComponent&);
};

} // namespace GUI

#endif

/* EOF */
