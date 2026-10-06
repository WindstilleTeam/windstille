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
#include <wstdisplay/view.hpp>

#include "engine/camera.hpp"
#include "util/currenton.hpp"

namespace wstinput {
class Controller;
} // namespace wstinput

namespace windstille {

class SceneContext;
class Sector;

/** This class is the gui component which renders the world to the
    screen */
class View : public Currenton<View>
{
private:
  wstdisplay::View m_view;
  Camera camera;
  float    m_debug_zoom;
  glm::vec2 m_debug_transform;

public:
  View();

  /** Maps the world to the pixels of the window's drawable, which
      can be larger than the window on high-DPI screens */
  wstdisplay::View const& get_view() const { return m_view; }

  /** @return the rectangle which represents the currently visible
      area, everything outside of it doesn't have to be drawn */
  geom::frect get_clip_rect() const;

  /** \a point is in window coordinates, e.g. a mouse position */
  glm::vec2 screen_to_world(glm::vec2 const& point) const;

  void draw(SceneContext& sc, Sector& sector);
  void update(float delta);

private:
  void update_view();
};

} // namespace windstille

#endif

/* EOF */
