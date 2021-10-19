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

#ifndef HEADER_WINDSTILLE_SCREEN_VIEW_HPP
#define HEADER_WINDSTILLE_SCREEN_VIEW_HPP

#include <glm/glm.hpp>
#include <wstdisplay/graphic_context_state.hpp>

#include "engine/camera.hpp"
#include "util/currenton.hpp"

namespace wstinput {
class Controller;
} // namespace wstinput

/** This class is the gui component which renders the world to the
    screen */
class View : public Currenton<View>
{
private:
  wstdisplay::GraphicContextState state;
  Camera camera;

  float    m_debug_zoom;
  glm::vec2 m_debug_transform;

public:
  View();

  wstdisplay::GraphicContextState get_gc_state() { return state; }

  /** @return the rectangle which represents the currently visible
      area, everything outside of it doesn't have to be drawn */
  geom::frect get_clip_rect();
  glm::vec2 screen_to_world(const glm::vec2& point);

  void draw(wstdisplay::SceneContext& sc, Sector& sector);
  void update(float delta);
};

#endif

/* EOF */
