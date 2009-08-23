/*
**  Windstille - A Sci-Fi Action-Adventure Game
**  Copyright (C) 2005 Ingo Ruhnke <grumbel@gmx.de>
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

#include <assert.h>
#include "input/controller.hpp"
#include "font/fonts.hpp"
#include "display/display.hpp"
#include "tab_component.hpp"

namespace gui {

TabComponent::TabComponent(Component* parent)
  : Component(parent),
    tabs(),
    current_tab(0)
{
  assert(0);
}

TabComponent::TabComponent(const Rectf& rect, Component* parent)
  : Component(rect, parent),
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

  float tab_width = rect.get_width()/tabs.size();
  for(int i = 0; i != int(tabs.size()); ++i)
    {
      Rectf tab_rect(Vector2f(rect.left + tab_width * i + 10,
                            rect.top),
                     Sizef(tab_width - 20, Fonts::current()->vera20->get_height() + 6));

      if (i == current_tab)
        Display::fill_rounded_rect(tab_rect, 5.0f, Color(1.0f, 1.0f, 1.0f, 0.5f));
      else
        Display::fill_rounded_rect(tab_rect, 5.0f, Color(0.0f, 0.0f, 0.0f, 0.5f));

      Display::draw_rounded_rect(tab_rect, 5.0f, Color(1.0f, 1.0f, 1.0f, 0.5f));

      Fonts::current()->vera20->draw_center(Vector2f(rect.left + tab_width * i + tab_width/2,
                                                     rect.top + Fonts::current()->vera20->get_height()),
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

      for(InputEventLst::const_iterator i = controller.get_events().begin(); i != controller.get_events().end(); ++i) 
        {
          if (i->type == BUTTON_EVENT && i->button.down)
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
          else if (i->type == AXIS_EVENT)
            {
              if (i->axis.name == X_AXIS)
                {
                  if (i->axis.pos < 0)
                    {
                      if (current_tab == 0)
                        current_tab = tabs.size() - 1;
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

  int padding = 6;
  component->set_screen_rect(Rectf(rect.left + padding,
                                   rect.top  + padding + Fonts::current()->vera20->get_height() + 10,
                                   rect.right  - padding,
                                   rect.bottom - padding
                                   ));
}

} // namespace gui

/* EOF */
