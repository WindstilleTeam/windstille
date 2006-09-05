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

#ifndef HEADER_WINDSTILLE_GUI_TAB_COMPONENT_HPP
#define HEADER_WINDSTILLE_GUI_TAB_COMPONENT_HPP

#include <string>
#include <vector>
#include "lisp/lisp.hpp"
#include "component.hpp"

namespace GUI {

/** */
class TabComponent : public Component
{
private:
  struct Tab 
  {
    std::string label;
    Component*  component;

    Tab() : component(0) {}
    
    Tab(const std::string& label_, Component* c)
      : label(label_), component(c) {}
  };

  typedef std::vector<Tab> Tabs;
  Tabs tabs;

  int current_tab;

public:
  TabComponent(const lisp::Lisp* lisp, Component* parent);
  TabComponent(const Rectf& rect, Component* parent);
  ~TabComponent();

  void draw();
  void update(float delta, const Controller& controller);

  void pack(const std::string& name, Component* component);
private:
  TabComponent (const TabComponent&);
  TabComponent& operator= (const TabComponent&);
};

} // namespace GUI

#endif

/* EOF */
