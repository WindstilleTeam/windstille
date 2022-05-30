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

#ifndef HEADER_WINDSTILLE_EDITOR_DECAL_SCALE_CONTROL_POINT_HPP
#define HEADER_WINDSTILLE_EDITOR_DECAL_SCALE_CONTROL_POINT_HPP

#include <glm/glm.hpp>
#include "editor/control_point.hpp"
#include "editor/decal_object_model.hpp"

namespace windstille {


class DecalScaleControlPoint : public ControlPoint
{
private:
  DecalObjectModel* object;
  float ctrl_angle;
  glm::vec2 orig_scale;
  bool x_scale;
  bool y_scale;

public:
  DecalScaleControlPoint(DecalObjectModel* object_, float ctrl_angle_, glm::vec2 const& pos_, bool
                         x_scale_ = true, bool y_scale_ = true);

  void on_move_start(GdkEventButton* event) override;
  void on_move_update(GdkEventMotion* event, glm::vec2 const& offset_) override;
  void on_move_end(GdkEventButton* event, glm::vec2 const& offset_) override;

  void draw(wstdisplay::SceneContext& sc) override;

private:
  DecalScaleControlPoint (DecalScaleControlPoint const&);
  DecalScaleControlPoint& operator= (DecalScaleControlPoint const&);
};


} // namespace windstille

#endif

/* EOF */
