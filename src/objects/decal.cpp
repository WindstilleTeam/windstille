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

#include "objects/decal.hpp"

#include <glm/trigonometric.hpp>
#include <wstdisplay/canvas.hpp>
#include <wstdisplay/surface_manager.hpp>

#include "app/app.hpp"
#include "display/scene_context.hpp"
#include "engine/sector.hpp"

namespace windstille {

Decal::Decal(ReaderMapping const& reader) :
  m_surface(),
  m_params(),
  m_layer(SceneContext::COLORMAP),
  pos()
{
  std::string path;
  glm::vec2 scale(1.0f, 1.0f);
  float    angle = 0.0f;

  bool hflip = false;
  bool vflip = false;

  int map_type = 0;
  reader.read("type", map_type);

  reader.read("pos",   pos);
  reader.read("path",  path);
  reader.read("scale", scale);
  reader.read("angle", angle);
  reader.read("vflip", vflip);
  reader.read("hflip", hflip);

  m_surface = g_app.surface().get(Pathname(path));

  // FIXME: Evil hardcoded constans, see edtior/decal_object_model.hpp
  switch(map_type)
  {
    case 0: m_layer = SceneContext::COLORMAP; break;
    case 1: m_layer = SceneContext::LIGHTMAP; break;
    case 2: m_layer = SceneContext::HIGHLIGHTMAP; break;
  }

  m_params
    .set_pos(pos)
    .set_anchor(geom::origin::CENTER)
    .set_angle(glm::degrees(angle))
    .set_hflip(hflip)
    .set_vflip(vflip)
    .set_scale(scale);
}

Decal::~Decal()
{
}

void
Decal::draw(SceneContext& sc)
{
  wstdisplay::Canvas& canvas = (m_layer == SceneContext::LIGHTMAP) ? sc.light() :
    (m_layer == SceneContext::HIGHLIGHTMAP) ? sc.highlight() : sc.color();

  wstdisplay::Canvas::Scope scope(canvas);
  canvas.set_z(0.0f);
  canvas.set_blend(m_layer == SceneContext::COLORMAP ? wstdisplay::Blend::Alpha : wstdisplay::Blend::Add);
  canvas.draw(m_surface, m_params);
}

void
Decal::update(float /*delta*/)
{
}

void
Decal::set_parent(GameObject* parent)
{
  Decal* decal = dynamic_cast<Decal*>(parent);
  if (decal)
  { // FIXME: Not going to work with double parenting
    pos += decal->pos;
    m_params.set_pos(pos);
  }
}

} // namespace windstille

/* EOF */
