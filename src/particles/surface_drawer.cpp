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

#include "particles/surface_drawer.hpp"

#include <cmath>
#include <iostream>

#include <glm/gtc/constants.hpp>
#include <wstdisplay/canvas.hpp>
#include <wstdisplay/surface_manager.hpp>

#include "particles/particle_system.hpp"
#include "util/file_reader.hpp"
#include "util/pathname.hpp"

namespace windstille {

namespace {

/** The particle files give GL blend factors, only the combinations
    for regular and additive blending are used */
wstdisplay::Blend blend_from_factors(std::string const& src, std::string const& dst)
{
  if (src == "src_alpha" && dst == "one_minus_src_alpha") {
    return wstdisplay::Blend::Alpha;
  } else if (src == "src_alpha" && dst == "one") {
    return wstdisplay::Blend::Add;
  } else if (src == "one" && dst == "zero") {
    return wstdisplay::Blend::Opaque;
  } else if (src == "dst_color" && dst == "zero") {
    return wstdisplay::Blend::Multiply;
  } else {
    std::cout << "SurfaceDrawer: unsupported blendfunc: '" << src << "', '" << dst << "'" << std::endl;
    return wstdisplay::Blend::Alpha;
  }
}

} // namespace

SurfaceDrawer::SurfaceDrawer(wstdisplay::Surface const& surface_) :
  surface(surface_),
  m_blend(wstdisplay::Blend::Alpha),
  m_vertices()
{
}

SurfaceDrawer::SurfaceDrawer(ReaderMapping const& props,
                             wstdisplay::SurfaceManager& surface_manager) :
  surface(),
  m_blend(wstdisplay::Blend::Alpha),
  m_vertices()
{
  std::string blendfunc_src_str = "src_alpha";
  std::string blendfunc_dst_str = "one_minus_src_alpha";
  std::string surface_file;

  props.read("image", surface_file);
  props.read("blendfunc-src", blendfunc_src_str);
  props.read("blendfunc-dst", blendfunc_dst_str);

  surface = surface_manager.get(Pathname(surface_file));
  m_blend = blend_from_factors(blendfunc_src_str, blendfunc_dst_str);
}

SurfaceDrawer::~SurfaceDrawer()
{
}

void
SurfaceDrawer::set_texture(wstdisplay::Surface const& surface_)
{
  surface = surface_;
}

void
SurfaceDrawer::draw(wstdisplay::Canvas& canvas, ParticleSystem const& psys) const
{
  geom::frect const& uv = surface.get_uv();

  m_vertices.clear();
  for(ParticleSystem::const_iterator i = psys.begin(); i != psys.end(); ++i)
  {
    if (i->t != -1.0f)
    {
      float p = 1.0f - psys.get_progress(i->t);
      surf::Color color(psys.get_color_start().r * p + psys.get_color_stop().r * (1.0f - p),
                        psys.get_color_start().g * p + psys.get_color_stop().g * (1.0f - p),
                        psys.get_color_start().b * p + psys.get_color_stop().b * (1.0f - p),
                        psys.get_color_start().a * p + psys.get_color_stop().a * (1.0f - p));

      float scale  = psys.get_size_start() +
        psys.get_progress(i->t) * (psys.get_size_stop() - psys.get_size_start());

      float width  = surface.get_width()  * scale;
      float height = surface.get_height() * scale;

      // corner offsets of the rotated quad
      float x_rot = width/2;
      float y_rot = height/2;

      if (i->angle != 0)
      {
        float s = std::sin(glm::pi<float>() * i->angle/180.0f);
        float c = std::cos(glm::pi<float>() * i->angle/180.0f);
        x_rot = (width/2) * c - (height/2) * s;
        y_rot = (width/2) * s + (height/2) * c;
      }

      wstdisplay::PackedColor const packed = wstdisplay::pack_color(color);
      m_vertices.push_back(wstdisplay::Vertex{i->x - x_rot, i->y - y_rot, uv.left(),  uv.top(),    packed, {}});
      m_vertices.push_back(wstdisplay::Vertex{i->x + y_rot, i->y - x_rot, uv.right(), uv.top(),    packed, {}});
      m_vertices.push_back(wstdisplay::Vertex{i->x + x_rot, i->y + y_rot, uv.right(), uv.bottom(), packed, {}});
      m_vertices.push_back(wstdisplay::Vertex{i->x - y_rot, i->y + x_rot, uv.left(),  uv.bottom(), packed, {}});
    }
  }

  wstdisplay::Canvas::Scope scope(canvas);
  canvas.translate(psys.get_x_pos(), psys.get_y_pos());
  canvas.set_blend(m_blend);
  canvas.draw_quads(surface.get_texture(), m_vertices);
}

} // namespace windstille

/* EOF */
