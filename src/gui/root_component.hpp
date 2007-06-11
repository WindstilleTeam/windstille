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

#ifndef HEADER_WINDSTILLE_GUI_ROOT_COMPONENT_HPP
#define HEADER_WINDSTILLE_GUI_ROOT_COMPONENT_HPP

#include <vector>
#include "component.hpp"

namespace GUI {

/** */
class RootComponent : public Component
{
private:
  Component* child;
  typedef std::vector<Component*> Children;
  Children chidren;
public:
  RootComponent(const Rectf& rect);
  ~RootComponent();

  void draw();
  void update(float delta, const Controller& controller);

  /** Set the main chidren */
  void set_child(Component* child);
  void add_child(Component* child);

  Component* get_child() const { return child; }
  
  bool is_active() const;
private:
  RootComponent (const RootComponent&);
  RootComponent& operator= (const RootComponent&);
};

} // namespace GUI

#endif

/* EOF */
