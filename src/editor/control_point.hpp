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

#ifndef HEADER_WINDSTILLE_EDITOR_CONTROL_POINT_HPP
#define HEADER_WINDSTILLE_EDITOR_CONTROL_POINT_HPP

#include <memory>

#include <glm/glm.hpp>

#include <geom/geom.hpp>

#include <wstdisplay/fwd.hpp>
#include <wstdisplay/surface.hpp>
#include <wstdisplay/view.hpp>

namespace windstille {

class SceneContext;


class ControlPoint;
typedef std::shared_ptr<ControlPoint> ControlPointHandle;


class ControlPoint
{
public:
  static ControlPointHandle create(glm::vec2 const& pos);

protected:
  wstdisplay::Surface surface;
  glm::vec2  pos;
  glm::vec2  offset;

public:
  ControlPoint(wstdisplay::Surface const& surface, glm::vec2 const& pos);
  virtual ~ControlPoint();

  /** Draw the handle onto \a overlay, which is in screen
      coordinates, handles keep their size when zooming */
  virtual void draw(wstdisplay::Canvas& overlay, wstdisplay::View const& view);

  virtual geom::frect get_bounding_box() const;

  virtual void on_move_start();
  virtual void on_move_update(glm::vec2 const& offset);
  virtual void on_move_end(GdkEventButton* event, glm::vec2 const& offset);

protected:
  /** Draw the surface centered on pos, rotated by \a angle radians */
  void draw_handle(wstdisplay::Canvas& overlay, wstdisplay::View const& view, float angle) const;

private:
  ControlPoint(ControlPoint const&);
  ControlPoint& operator=(ControlPoint const&);
};


} // namespace windstille

#endif

/* EOF */
