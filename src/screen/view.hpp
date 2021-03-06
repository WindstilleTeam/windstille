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

#include "display/graphic_context_state.hpp"
#include "engine/camera.hpp"
#include "math/vector2f.hpp"
#include "util/currenton.hpp"

class Controller;
class SceneContext;

/** This class is the gui component which renders the world to the
    screen */
class View : public Currenton<View>
{
private:
  GraphicContextState state;
  Camera camera;

  float    m_debug_zoom;
  Vector2f m_debug_transform;

public:
  View();

  GraphicContextState get_gc_state() { return state; }

  /** @return the rectangle which represents the currently visible
      area, everything outside of it doesn't have to be drawn */
  Rectf get_clip_rect();
  Vector2f screen_to_world(const Vector2f& point);

  void draw(SceneContext& gc, Sector& sector);
  void update(float delta);
};

#endif

/* EOF */
