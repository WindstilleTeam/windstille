/*
**  Windstille - A Sci-Fi Action-Adventure Game
**  Copyright (C) 2009 Ingo Ruhnke <grumbel@gmail.com>
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

#include <glm/glm.hpp>
#include "editor/windstille_widget.hpp"
#include "editor/scroll_tool.hpp"

namespace windstille {


ScrollTool::ScrollTool() :
  orig_state(),
  orig_click(),
  mode(NO_MODE)
{
}

void
ScrollTool::mouse_down(GdkEventButton* event, WindstilleWidget& wst)
{
  orig_state = wst.get_state().clone();
  orig_click = orig_state.screen_to_world(glm::vec2(static_cast<float>(event->x), static_cast<float>(event->y))).as_vec();
  mode = SCROLLING;
  wst.queue_draw();
}

void
ScrollTool::mouse_move(GdkEventMotion* event, WindstilleWidget& wst)
{
  if (mode == SCROLLING)
  {
    glm::vec2 offset = orig_click - orig_state.screen_to_world(glm::vec2(static_cast<float>(event->x), static_cast<float>(event->y))).as_vec();
    wst.get_state().set_pos(orig_state.get_pos().as_vec() + offset);
    wst.queue_draw();
  }
}

void
ScrollTool::mouse_up(GdkEventButton* event, WindstilleWidget& wst)
{
  if (mode == SCROLLING)
  {
    glm::vec2 offset = orig_click - orig_state.screen_to_world(glm::vec2(static_cast<float>(event->x), static_cast<float>(event->y))).as_vec();
    wst.get_state().set_pos(orig_state.get_pos().as_vec() + offset);
    mode = NO_MODE;
    wst.queue_draw();
  }
}


} // namespace windstille

/* EOF */
