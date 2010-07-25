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

#include "editor/zoom_tool.hpp"

#include "display/scene_context.hpp"
#include "display/color.hpp"
#include "editor/windstille_widget.hpp"

ZoomTool::ZoomTool() :
  click_pos(),
  mouse_pos(),
  mode(NO_MODE)
{
}

void
ZoomTool::mouse_down (GdkEventButton* event, WindstilleWidget& wst)
{
  if (mode == NO_MODE)
  {
    mouse_pos = click_pos = wst.get_state().screen_to_world(Vector2f(static_cast<float>(event->x), static_cast<float>(event->y)));

    mode = RECT_MODE;

    wst.queue_draw();
  }
}

void
ZoomTool::mouse_move(GdkEventMotion* event, WindstilleWidget& wst)
{
  if (mode == RECT_MODE)
  {
    mouse_pos = wst.get_state().screen_to_world(Vector2f(static_cast<float>(event->x), static_cast<float>(event->y)));
    wst.queue_draw();
  }
}

void
ZoomTool::mouse_up(GdkEventButton* /*event*/, WindstilleWidget& wst)
{
  if (mode == RECT_MODE)
  {
    Rectf rect(click_pos, mouse_pos);
    rect.normalize();
    wst.get_state().zoom_to(rect);

    mode = NO_MODE;

    wst.queue_draw();
  }
}

void
ZoomTool::mouse_right_down(GdkEventButton* /*event*/, WindstilleWidget& wst)
{
  wst.on_zoom_out();
}

void
ZoomTool::draw(SceneContext& sc)
{
  if (mode == RECT_MODE)
  {
    Rectf rect(click_pos, mouse_pos);
    rect.normalize();

    sc.control().fill_rect(rect, Color(1.0f, 1.0f, 0.0f, 0.25));
    sc.control().draw_rect(rect, Color(1.0f, 1.0f, 0.0f)); 
  }
}

/* EOF */
