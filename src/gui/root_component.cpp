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

#include <algorithm>

#include <wstinput/controller.hpp>

#include "app/console.hpp"
#include "gui/root_component.hpp"

namespace gui {

RootComponent::RootComponent(const Rectf& rect_)
  : Component(rect_, nullptr),
    focus(nullptr),
    children()
{
  set_active(true);
}

RootComponent::~RootComponent()
{
  for(Children::iterator i = children.begin(); i != children.end(); ++i)
    delete *i;
  children.clear();
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
    ConsoleLog << "Error: Need to add_child() first befor calling set_focus()" << std::endl;
  }
}

} // namespace gui

/* EOF */
