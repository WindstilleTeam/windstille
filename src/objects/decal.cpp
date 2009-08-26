/*
**  Windstille - A Sci-Fi Action-Adventure Game
**  Copyright (C) 2009 Ingo Ruhnke <grumbel@gmx.de>
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

#include "objects/decal.hpp"

#include "engine/sector.hpp"
#include "scenegraph/scene_graph.hpp"
#include "display/surface_drawing_request.hpp"
#include "display/surface_drawing_parameters.hpp"

Decal::Decal(const FileReader& reader)
{
  std::string path;
  Vector2f pos;
  Vector2f scale(1.0f, 1.0f);
  float    angle = 0.0f;

  bool hflip = false;
  bool vflip = false;

  reader.get("pos",   pos);
  reader.get("path",  path);
  reader.get("scale", scale);
  reader.get("angle", angle);
  reader.get("vflip", vflip);
  reader.get("hflip", hflip);

  Surface surface = Surface(Pathname(path));

  SurfaceDrawingParameters params;
  boost::shared_ptr<SurfaceDrawingRequest> drawable(new SurfaceDrawingRequest(surface,
                                                                              params
                                                                              .set_pos(pos)
                                                                              .set_angle(angle)
                                                                              .set_hflip(hflip)
                                                                              .set_vflip(vflip)
                                                                              .set_scale(scale),
                                                                              0, 
                                                                              Matrix::identity()));

  Sector::current()->get_scene_graph().add_drawable(drawable);
}

Decal::~Decal()
{
}
  
void
Decal::draw(SceneContext& /*context*/)
{
}

void
Decal::update(float /*delta*/)
{
}

/* EOF */
