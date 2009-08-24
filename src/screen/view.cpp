/*
**  Windstille - A Sci-Fi Action-Adventure Game
**  Copyright (C) 2000,2005 Ingo Ruhnke <grumbel@gmx.de>
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

#include "display/display.hpp"
#include "collision/collision_engine.hpp"
#include "objects/player.hpp"
#include "engine/sector.hpp"
#include "input/input_manager.hpp"
#include "screen/view.hpp"

View::View()
  : state(Display::get_width(), Display::get_height()),
    camera(),
    zoom(1.0), 
    transform(0, 0)
{
}

void
View::draw (SceneContext& sc)
{
  if (camera.get_zoom() == 1.0)
    {
      state.set_pos(Vector2f(camera.get_pos().x, camera.get_pos().y));
    }
  else
    {
      state.set_pos(camera.get_pos());
    }

  state.set_zoom(camera.get_zoom() + (zoom - 1.0f));
  state.set_pos(state.get_pos() + Vector2f(transform.x, transform.y));

  state.push(sc);

  Sector::current()->draw(sc);

  if (collision_debug)
    Sector::current()->get_collision_engine()->draw(sc.highlight());

  state.pop(sc);
}

void
View::update (float delta)
{
  camera.update(delta);

  Uint8 *keystate = SDL_GetKeyState(NULL);

  if (keystate[SDLK_KP_PLUS])
    zoom *= 1.0f + delta;

  if (keystate[SDLK_KP_MINUS])
    zoom *= 1.0f - delta;

  const Controller& controller = InputManager::current()->get_controller();

  if (controller.get_button_state(DEBUG_BUTTON))
    {
      if (controller.get_button_state(VIEW_CENTER_BUTTON)) 
        {
          transform = Vector2f(0, 0);
          zoom = 1.0;
        }

      transform.x += 1.5f * controller.get_axis_state(X2_AXIS) / zoom;
      transform.y += 1.5f * controller.get_axis_state(Y2_AXIS) / zoom;
    }
}

Rectf
View::get_clip_rect()
{
  return state.get_clip_rect();
}

Vector2f
View::screen_to_world(const Vector2f& point)
{
  return state.screen_to_world(point);
}

/* EOF */
