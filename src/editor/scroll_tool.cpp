/*
**  Windstille - A Sci-Fi Action-Adventure Game
**  Copyright (C) 2009 Ingo Ruhnke <grumbel@gmx.de>
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

#include "math/vector2f.hpp"
#include "windstille_widget.hpp"
#include "scroll_tool.hpp"

ScrollTool::ScrollTool()
  : orig_state(),
    orig_click(),
    mode(NO_MODE)
{
}

void
ScrollTool::mouse_down(GdkEventButton* event, WindstilleWidget& wst)
{
  orig_state = wst.get_state().clone();
  orig_click = orig_state.screen_to_world(Vector2f(static_cast<float>(event->x), static_cast<float>(event->y)));
  mode = SCROLLING;
  wst.queue_draw();
}

void
ScrollTool::mouse_move(GdkEventMotion* event, WindstilleWidget& wst)
{
  if (mode == SCROLLING)
    {
      Vector2f offset = orig_click - orig_state.screen_to_world(Vector2f(static_cast<float>(event->x), static_cast<float>(event->y)));
      wst.get_state().set_pos(orig_state.get_pos() + offset);
      wst.queue_draw();
    }
}

void
ScrollTool::mouse_up(GdkEventButton* event, WindstilleWidget& wst)
{
  if (mode == SCROLLING)
    {
      Vector2f offset = orig_click - orig_state.screen_to_world(Vector2f(static_cast<float>(event->x), static_cast<float>(event->y)));
      wst.get_state().set_pos(orig_state.get_pos() + offset);
      mode = NO_MODE;
      wst.queue_draw();
    }
}

/* EOF */
