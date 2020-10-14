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

#include "gui/tab_component.hpp"

#include <wstinput/controller.hpp>

#include "app/app.hpp"
#include "app/controller_def.hpp"
#include "display/display.hpp"
#include "font/fonts.hpp"
#include "util/log.hpp"

namespace gui {

TabComponent::TabComponent(Component* parent_)
  : Component(parent_),
    tabs(),
    current_tab(0)
{
  log_error("not implemented");
}

TabComponent::TabComponent(const geom::frect& rect_, Component* parent_)
  : Component(rect_, parent_),
    tabs(),
    current_tab(0)
{
}

TabComponent::~TabComponent()
{
}

void
TabComponent::draw()
{
  if (tabs.empty()) return;

  float tab_width = rect.width() / static_cast<float>(tabs.size());
  for(int i = 0; i != int(tabs.size()); ++i)
  {
    geom::frect tab_rect(glm::vec2(rect.left() + tab_width * static_cast<float>(i) + 10.0f,
                            rect.top()),
                   geom::fsize(tab_width - 20.0f, static_cast<float>(g_app.fonts().vera20->get_height()) + 6.0f));

    if (i == current_tab)
      Display::fill_rounded_rect(tab_rect, 5.0f, Color(1.0f, 1.0f, 1.0f, 0.5f));
    else
      Display::fill_rounded_rect(tab_rect, 5.0f, Color(0.0f, 0.0f, 0.0f, 0.5f));

    Display::draw_rounded_rect(tab_rect, 5.0f, Color(1.0f, 1.0f, 1.0f, 0.5f));

    g_app.fonts().vera20->draw_center(glm::vec2(rect.left() + tab_width * static_cast<float>(i) + tab_width/2,
                                                   rect.top() + static_cast<float>(g_app.fonts().vera20->get_height())),
                                          tabs[i].label,
                                          tabs[current_tab].component->is_active()
                                          ? Color(1.0f, 1.0f, 1.0f, 0.5f)
                                          : Color(1.0f, 1.0f, 1.0f, 1.0f));
  }

  tabs[current_tab].component->draw();
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
  component->set_screen_rect(geom::frect(rect.left() + padding,
                                   rect.top()  + padding + static_cast<float>(g_app.fonts().vera20->get_height()) + 10.0f,
                                   rect.right()  - padding,
                                   rect.bottom() - padding
                               ));
}

} // namespace gui

/* EOF */
