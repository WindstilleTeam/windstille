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

#include <iostream>
#include <algorithm>
#include "console.hpp"
#include "input/controller.hpp"
#include "root_component.hpp"

namespace gui {

RootComponent::RootComponent(const Rectf& rect)
  : Component(rect, 0),
    focus(0)
{
  set_active(true);
}

RootComponent::~RootComponent()
{
}

void
RootComponent::draw()
{
  for(Children::iterator i = children.begin(); i != children.end(); ++i)
    {
      (*i)->draw();
    }
}

void
RootComponent::update(float delta, const Controller& controller)
{
  for(Children::iterator i = children.begin(); i != children.end(); ++i)
    {
      if (*i == focus)
        (*i)->update(delta, controller);
      else
        (*i)->update(delta, Controller());
    }
}

bool
RootComponent::is_active() const
{
  if (focus)
    return focus->is_active();
  else
    return false;
}

void
RootComponent::add_child(Component* child)
{
  focus = child;
  children.push_back(child);
}

void
RootComponent::set_focus(Component* child_)
{
  Children::iterator i = std::find(children.begin(), children.end(), child_);
  if (i != children.end())
    {
      focus = child_;
      focus->set_active(true);
    }
  else
    {
      console << "Error: Need to add_child() first befor calling set_focus()" << std::endl;
    }
}

} // namespace gui

/* EOF */
