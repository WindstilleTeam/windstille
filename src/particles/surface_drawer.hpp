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

#ifndef HEADER_WINDSTILLE_PARTICLES_SURFACE_DRAWER_HPP
#define HEADER_WINDSTILLE_PARTICLES_SURFACE_DRAWER_HPP

#include <vector>

#include <wstdisplay/blend.hpp>
#include <wstdisplay/fwd.hpp>
#include <wstdisplay/surface.hpp>
#include <wstdisplay/vertex.hpp>

#include "particles/drawer.hpp"
#include "util/file_reader.hpp"

namespace windstille {

/** Draws each particle as a rotated, scaled and colored image */
class SurfaceDrawer : public Drawer
{
private:
  wstdisplay::Surface surface;
  wstdisplay::Blend m_blend;

  /** Scratch buffer, reused between frames */
  mutable std::vector<wstdisplay::Vertex> m_vertices;

public:
  SurfaceDrawer(ReaderMapping const& props, wstdisplay::SurfaceManager& surface_manager);
  SurfaceDrawer(wstdisplay::Surface const& surface);
  ~SurfaceDrawer() override;

  void set_texture(wstdisplay::Surface const& surface);
  void set_blend(wstdisplay::Blend blend) { m_blend = blend; }

  void draw(wstdisplay::Canvas& canvas, ParticleSystem const& psys) const override;
};

} // namespace windstille

#endif

/* EOF */
