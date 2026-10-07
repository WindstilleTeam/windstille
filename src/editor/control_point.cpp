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

#include <iostream>

#include <glm/gtx/io.hpp>

#include "display/scene_context.hpp"
#include <glm/trigonometric.hpp>
#include <wstdisplay/canvas.hpp>
#include <wstdisplay/draw_params.hpp>
#include <wstdisplay/surface_manager.hpp>
#include "editor/app.hpp"
#include "editor/control_point.hpp"
#include "util/pathname.hpp"

namespace windstille {

ControlPointHandle
ControlPoint::create(glm::vec2 const& pos)
{
  return ControlPointHandle(new ControlPoint(g_app.surface().get(Pathname("editor/handle.png")), pos));
}


ControlPoint::ControlPoint(wstdisplay::Surface const& surface_, glm::vec2 const& pos_) :
  surface(surface_),
  pos(pos_),
  offset()
{
}

ControlPoint::~ControlPoint()
{
}

void
ControlPoint::on_move_start()
{
}

void
ControlPoint::on_move_update(glm::vec2 const& offset_)
{
  offset = offset_;
  std::cout << "on_move_update: " << offset << std::endl;
}

void
ControlPoint::on_move_end(glm::vec2 const& offset_)
{
  offset = offset_;
  offset = glm::vec2();
}

geom::frect
ControlPoint::get_bounding_box() const
{
  return geom::frect(pos - glm::vec2(11.0f, 11.0f), geom::fsize(22.0f, 22.0f));
}

void
ControlPoint::draw(wstdisplay::Canvas& overlay, wstdisplay::View const& view)
{
  draw_handle(overlay, view, 0.0f);
}

void
ControlPoint::draw_handle(wstdisplay::Canvas& overlay, wstdisplay::View const& view, float angle) const
{
  overlay.draw(surface,
               wstdisplay::DrawParams()
               .set_pos(view.world_to_screen(geom::fpoint(pos)))
               .set_anchor(geom::origin::CENTER)
               .set_angle(glm::degrees(angle)));
}


} // namespace windstille

/* EOF */
