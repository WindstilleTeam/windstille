/*
**  Windstille - A Sci-Fi Action-Adventure Game
**  Copyright (C) 2000,2005 Ingo Ruhnke <grumbel@gmail.com>
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

#include <SDL.h>

#include <wstinput/input_manager.hpp>

#include "app/app.hpp"
#include "app/controller_def.hpp"
#include "collision/collision_engine.hpp"
#include <wstdisplay/graphics_context.hpp>
#include <wstdisplay/opengl_window.hpp>
#include "engine/sector.hpp"
#include "objects/player.hpp"
#include "screen/view.hpp"

namespace windstille {

View::View()
  : state(g_app.window().get_gc().size().width(),
          g_app.window().get_gc().size().height()),
    camera(),
    m_debug_zoom(1.0),
    m_debug_transform(0, 0)
{
}

void
View::draw(wstdisplay::SceneContext& sc, Sector& sector)
{
  state.set_zoom(camera.get_zoom() + (m_debug_zoom - 1.0f));
  state.set_pos(camera.get_pos() + m_debug_transform);

  state.push(sc);

  sector.draw(sc);

  if (collision_debug)
    sector.get_collision_engine()->draw(sc.highlight());

  state.pop(sc);
}

void
View::update (float delta)
{
  camera.update(delta);

  Uint8 const* keystate = SDL_GetKeyboardState(nullptr);

  if (keystate[SDL_SCANCODE_KP_PLUS])
    m_debug_zoom *= 1.0f + delta;

  if (keystate[SDL_SCANCODE_KP_MINUS])
    m_debug_zoom *= 1.0f - delta;

  wstinput::Controller const& controller = g_app.input().get_controller();

  if (controller.get_button_state(DEBUG_BUTTON))
  {
    if (controller.get_button_state(VIEW_CENTER_BUTTON))
    {
      m_debug_transform = glm::vec2(0, 0);
      m_debug_zoom = 1.0;
    }

    m_debug_transform.x += 1000.0f * controller.get_axis_state(X2_AXIS) * delta / m_debug_zoom;
    m_debug_transform.y += 1000.0f * controller.get_axis_state(Y2_AXIS) * delta / m_debug_zoom;
  }
}

geom::frect
View::get_clip_rect()
{
  return state.get_clip_rect();
}

glm::vec2
View::screen_to_world(glm::vec2 const& point)
{
  return state.screen_to_world(point).as_vec();
}

} // namespace windstille

/* EOF */
