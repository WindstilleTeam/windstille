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
#include <wstdisplay/opengl_window.hpp>

#include "app/app.hpp"
#include "app/config.hpp"
#include "app/controller_def.hpp"
#include "collision/collision_engine.hpp"
#include "display/scene_context.hpp"
#include "engine/sector.hpp"
#include "objects/player.hpp"
#include "screen/view.hpp"

namespace windstille {

namespace {

/** Drawable pixels per window coordinate */
float pixel_ratio()
{
  float const window_width = static_cast<float>(g_app.window().get_size().width());
  float const drawable_width = static_cast<float>(g_app.window().get_drawable_size().width());
  return window_width > 0.0f ? drawable_width / window_width : 1.0f;
}

} // namespace

View::View()
  : m_view(g_app.window().get_drawable_size()),
    camera(),
    m_debug_zoom(1.0),
    m_debug_transform(0, 0)
{
}

void
View::update_view()
{
  // camera zoom (scripts/paths) × user view-zoom (options, percent) × debug zoom
  float const user_zoom = static_cast<float>(config.get_int("view-zoom")) / 100.0f;
  float const z = camera.get_zoom() * user_zoom * m_debug_zoom;

  m_view.set_size(g_app.window().get_drawable_size());
  m_view.set_zoom((z > 0.01f ? z : 0.01f) * pixel_ratio());
  glm::vec2 const pos = camera.get_pos() + m_debug_transform;
  m_view.set_pos(geom::fpoint(pos.x, pos.y));
}

void
View::draw(SceneContext& sc, Sector& sector)
{
  update_view();

  sector.draw(sc);

  if (collision_debug)
    sector.get_collision_engine()->draw(sc.highlight());
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
View::get_clip_rect() const
{
  return m_view.get_clip_rect();
}

glm::vec2
View::screen_to_world(glm::vec2 const& point) const
{
  float const ratio = pixel_ratio();
  return m_view.screen_to_world(geom::fpoint(point.x * ratio, point.y * ratio)).as_vec();
}

} // namespace windstille

/* EOF */
