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
#include <gdkmm/event.h>

#include "display/surface.hpp"
#include "math/rect.hpp"
#include "math/vector2f.hpp"

class SceneContext;
class ControlPoint;
typedef std::shared_ptr<ControlPoint> ControlPointHandle;

class ControlPoint
{
public:
  static ControlPointHandle create(const Vector2f& pos);

protected:
  SurfacePtr surface;
  Vector2f  pos;
  Vector2f  offset;

public:
  ControlPoint(SurfacePtr surface, const Vector2f& pos);
  virtual ~ControlPoint();

  virtual void draw(SceneContext& sc);

  virtual Rectf get_bounding_box() const;

  virtual void on_move_start(GdkEventButton* event);
  virtual void on_move_update(GdkEventMotion* event, const Vector2f& offset);
  virtual void on_move_end(GdkEventButton* event, const Vector2f& offset);

private:
  ControlPoint(const ControlPoint&);
  ControlPoint& operator=(const ControlPoint&);
};

#endif

/* EOF */
