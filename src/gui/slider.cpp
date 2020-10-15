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

#include "gui/slider.hpp"

#include <wstinput/controller.hpp>

#include "app/controller_def.hpp"
#include "display/color.hpp"
#include "display/display.hpp"
#include "gui/list_view.hpp"

namespace gui {

Slider::Slider(Component* parent_)
  : Component(parent_),
    min(0),
    max(100),
    step(10),
    pos (50),
    horizontal(true)
{
}

Slider::~Slider()
{
}

int
Slider::set_minimum(int min_)
{
  return min = min_;
}

int
Slider::set_maximum(int max_)
{
  return max = max_;
}

void
Slider::set_range(int min_, int max_)
{
  min = min_;
  max = max_;
}

void
Slider::set_step(int step_)
{
  step = step_;
}

void
Slider::draw(GraphicsContext& gc)
{
  gc.fill_rect(rect, Color(0.0f, 0.0f, 0.0f, 0.5f));
  gc.draw_rect(rect, Color(1.0f, 1.0f, 1.0f, 0.5f));

  float width = 30.0f;
  geom::frect slider_rect(glm::vec2(rect.left() + (rect.width()-width) * (static_cast<float>(pos-min)/static_cast<float>(max - min)),
                             rect.top() + 2.0f),
                    geom::fsize(width, rect.height() - 4.0f));
  if (is_active())
  {
    gc.fill_rect(slider_rect, Color(1.0f, 1.0f, 1.0f, 0.5f));
    gc.draw_rect(slider_rect, Color(1.0f, 1.0f, 1.0f, 1.0f));
  }
  else
  {
    gc.draw_rect(slider_rect, Color(1.0f, 1.0f, 1.0f, 0.5f));
  }
}

void
Slider::update(float /*delta*/, const Controller& controller)
{
  for(auto i = controller.get_events().begin(); i != controller.get_events().end(); ++i)
  {
    if (i->type == wstinput::BUTTON_EVENT && i->button.down)
    {
      if (i->button.name == OK_BUTTON || i->button.name == ENTER_BUTTON)
      {

      }
      else if (i->button.name == CANCEL_BUTTON || i->button.name == ESCAPE_BUTTON)
      {
        set_active(false);
      }
    }
    else if (i->type == wstinput::AXIS_EVENT)
    {
      if (i->axis.name == X_AXIS)
      {
        if (i->axis.pos < 0)
        {
          pos -= step;
          if (pos < min)
            pos = min;
        }
        else if (i->axis.pos > 0)
        {
          pos += step;
          if (pos > max)
            pos = max;
        }
      }
    }
  }
}

int
Slider::get_pos() const
{
  return pos;
}

void
Slider::set_pos(int pos_)
{
  pos = pos_;
}

} // namespace gui

/* EOF */
