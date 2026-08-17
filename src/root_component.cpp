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

#include <logmich/log.hpp>
#include <wstinput/controller.hpp>

#include "root_component.hpp"

namespace wstgui {

RootComponent::RootComponent(Style& style) :
  Component(nullptr),
  m_focus(nullptr),
  m_children(),
  m_style(style)
{
  set_active(true);
}

RootComponent::~RootComponent()
{
}

void
RootComponent::draw(wstdisplay::GraphicsContext& gc)
{
  for (auto i = m_children.begin(); i != m_children.end(); ++i)
  {
    (*i)->draw(gc);
  }
}

void
RootComponent::update(float delta, const Controller& controller)
{
  for (auto i = m_children.begin(); i != m_children.end(); ++i)
  {
    if (i->get() == m_focus)
      (*i)->update(delta, controller);
    else
      (*i)->update(delta, Controller());
  }
}

bool
RootComponent::is_active() const
{
  return true;

#if 0
  if (m_focus)
    return m_focus->is_active();
  else
    return false;
#endif
}

void
RootComponent::add_child(std::unique_ptr<Component> child)
{
  m_focus = child.get();
  m_children.emplace_back(std::move(child));
}

void
RootComponent::set_focus(Component* child_)
{
  auto const i = std::find_if(m_children.begin(), m_children.end(),
                              [child_](std::unique_ptr<Component> const& it){ return it.get() == child_; });
  if (i != m_children.end())
  {
    m_focus = child_;
    m_focus->set_active(true);
  }
  else
  {
    log_error("Error: Need to add_child() first befor calling set_m_focus()");
  }
}

Style&
RootComponent::get_style() const
{
  return m_style;
}

Component*
RootComponent::query(geom::fpoint const& pos) const
{
  for (auto it = m_children.rbegin(); it != m_children.rend(); ++it) {
    auto const& component = *it;

    geom::frect const rect = component->geometry();
    if (geom::contains(rect, pos)) {
      return component.get();
    }
  }

  return nullptr;
}

} // namespace wstgui

/* EOF */
