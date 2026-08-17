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

#include <wstdisplay/graphics_context.hpp>

#include "group_component.hpp"
#include "style.hpp"

namespace wstgui {

GroupComponent::GroupComponent(const std::string& title, Component* parent) :
  Component(parent),
  m_title(title),
  m_child()
{
}

GroupComponent::~GroupComponent()
{
}

void
GroupComponent::draw(wstdisplay::GraphicsContext& gc)
{
  gc.fill_rounded_rect(m_geometry, 5.0f, surf::Color(0.0f, 0.0f, 0.0f, 0.7f));
  gc.draw_rounded_rect(m_geometry, 5.0f, surf::Color(1.0f, 1.0f, 1.0f, 0.5f));

  if (!m_title.empty())
  {
    wstdisplay::TTFFont* font = get_style().get_font();
    font->draw_center(gc,
                      glm::vec2(m_geometry.left() + m_geometry.width() / 2.0f,
                                m_geometry.top()  + static_cast<float>(font->get_height()) + 5.0f),
                      m_title, surf::Color(1.0f, 1.0f, 1.0f));

    gc.fill_rect(geom::frect(m_geometry.left()  + 8.0f, m_geometry.top() + static_cast<float>(font->get_height()) + 16.0f,
                             m_geometry.right() - 8.0f, m_geometry.top() + static_cast<float>(font->get_height()) + 18.0f),
                 surf::Color(1.0f, 1.0f, 1.0f, 0.5f));
  }

  if (m_child) {
    m_child->draw(gc);
  }
}

void
GroupComponent::update(float delta, const Controller& controller)
{
  if (m_child) {
    m_child->update(delta, controller);
  }
}

void
GroupComponent::pack(std::unique_ptr<Component> component)
{
  m_child = std::move(component);
  m_child->set_active(true);
}

geom::frect
GroupComponent::get_child_rect() const
{
  float const padding = 6.0f;

  return geom::frect(m_geometry.left() + padding,
                     m_geometry.top() + padding + (m_title.empty() ?
                                                   0.0f :
                                                   static_cast<float>(get_style().get_font()->get_height()) + 18.0f),
                     m_geometry.right()  - padding,
                     m_geometry.bottom() - padding);
}

void
GroupComponent::set_geometry(geom::frect const& rect)
{
  Component::set_geometry(rect);
  if (m_child) {
    m_child->set_geometry(get_child_rect());
  }
}

bool
GroupComponent::is_active() const
{
  if (m_child) {
    return m_child->is_active();
  } else {
    return false;
  }
}

} // namespace wstgui

/* EOF */
