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
#include "input/controller.hpp"
#include "root_component.hpp"

namespace GUI {

RootComponent::RootComponent(const Rectf& rect)
  : Component(rect, 0),
    child(0)
{
  set_active(true);
}

RootComponent::~RootComponent()
{
}

void
RootComponent::set_child(Component* child_)
{
  child = child_;
  child->set_active(true);
}

void
RootComponent::draw()
{
  for(Children::iterator i = chidren.begin(); i != chidren.end(); ++i)
    {
      (*i)->draw();
    }

  if (child)
    child->draw();
}

void
RootComponent::update(float delta, const Controller& controller)
{
  if (child)
    child->update(delta, controller);
  
  for(Children::iterator i = chidren.begin(); i != chidren.end(); ++i)
    {
      (*i)->update(delta, Controller());
    }
}

bool
RootComponent::is_active() const
{
  if (child)
    return child->is_active();
  else
    return false;
}

void
RootComponent::add_child(Component* child)
{
  chidren.push_back(child);
}

} // namespace GUI

/* EOF */
