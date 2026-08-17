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

#include "tab_component.hpp"

#include <wstinput/controller.hpp>

#include <logmich/log.hpp>
#include <wstdisplay/graphics_context.hpp>

#include "controller_def.hpp"

namespace wstgui {

TabComponent::TabComponent(Component* parent_)
  : Component(parent_),
    tabs(),
    current_tab(0)
{
}

TabComponent::~TabComponent()
{
}

void
TabComponent::draw(wstdisplay::GraphicsContext& gc)
{
  if (tabs.empty()) return;

  float tab_width = m_geometry.width() / static_cast<float>(tabs.size());
  for(int i = 0; i != int(tabs.size()); ++i)
  {
    geom::frect tab_rect(glm::vec2(m_geometry.left() + tab_width * static_cast<float>(i) + 10.0f,
                                   m_geometry.top()),
                         geom::fsize(tab_width - 20.0f, static_cast<float>(get_style().get_font()->get_height()) + 6.0f));

    if (i == current_tab)
      gc.fill_rounded_rect(tab_rect, 5.0f, surf::Color(1.0f, 1.0f, 1.0f, 0.5f));
    else
      gc.fill_rounded_rect(tab_rect, 5.0f, surf::Color(0.0f, 0.0f, 0.0f, 0.5f));

    gc.draw_rounded_rect(tab_rect, 5.0f, surf::Color(1.0f, 1.0f, 1.0f, 0.5f));

    get_style().get_font()->draw_center(gc,
                                      glm::vec2(m_geometry.left() + tab_width * static_cast<float>(i) + tab_width/2,
                                                m_geometry.top() + static_cast<float>(get_style().get_font()->get_height())),
                                      tabs[i].label,
                                      tabs[current_tab].component->is_active()
                                      ? surf::Color(1.0f, 1.0f, 1.0f, 0.5f)
                                      : surf::Color(1.0f, 1.0f, 1.0f, 1.0f));
  }

  tabs[current_tab].component->draw(gc);
}

void
TabComponent::update(float delta, const Controller& controller)
{
  if (tabs.empty()) return;

  if (tabs[current_tab].component->is_active())
  {
    tabs[current_tab].component->update(delta, controller);
  }
  else
  {
    tabs[current_tab].component->update(delta, Controller());

    for(auto i = controller.get_events().begin(); i != controller.get_events().end(); ++i)
    {
      if (i->type == wstinput::BUTTON_EVENT && i->button.down)
      {
        if (i->button.name == OK_BUTTON || i->button.name == ENTER_BUTTON)
        {
          tabs[current_tab].component->set_active(true);
        }
        else if (i->button.name == CANCEL_BUTTON || i->button.name == ESCAPE_BUTTON)
        {
          tabs[current_tab].component->set_active(false);
          set_active(false);
        }
      }
      else if (i->type == wstinput::AXIS_EVENT)
      {
        if (i->axis.name == X_AXIS)
        {
          if (i->axis.pos < 0)
          {
            if (current_tab == 0)
              current_tab = static_cast<int>(tabs.size()) - 1;
            else
              current_tab -= 1;
          }
          else if (i->axis.pos > 0)
          {
            if (current_tab == int(tabs.size()) - 1)
              current_tab = 0;
            else
              current_tab += 1;
          }
        }
        else if (i->axis.name == Y_AXIS)
        {
          if (i->axis.pos < 0)
          {
          }
          else if (i->axis.pos > 0)
          {
            tabs[current_tab].component->set_active(true);
          }
        }
      }
    }
  }
}

void
TabComponent::pack(const std::string& name, Component* component)
{
  tabs.push_back(Tab(name, component));

  float padding = 6.0f;
  component->set_geometry(geom::frect(m_geometry.left() + padding,
                                   m_geometry.top()  + padding + static_cast<float>(get_style().get_font()->get_height()) + 10.0f,
                                   m_geometry.right()  - padding,
                                   m_geometry.bottom() - padding
                               ));
}

} // namespace wstgui

/* EOF */
