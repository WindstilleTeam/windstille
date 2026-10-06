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

#ifndef HEADER_WINDSTILLE_PARTICLES_SPARK_DRAWER_HPP
#define HEADER_WINDSTILLE_PARTICLES_SPARK_DRAWER_HPP

#include <vector>

#include <surf/color.hpp>
#include <wstdisplay/fwd.hpp>
#include <wstdisplay/vertex.hpp>

#include "util/file_reader.hpp"

#include "particles/drawer.hpp"

namespace windstille {

class ParticleSystem;

class SparkDrawer : public Drawer
{
private:
  surf::Color color;
  float width;
  /** Scratch buffer, reused between frames */
  mutable std::vector<wstdisplay::Vertex> m_vertices;

public:
  SparkDrawer(ReaderMapping const& props);

  void draw(wstdisplay::Canvas& canvas, ParticleSystem const& psys) const override;
};

} // namespace windstille

#endif

/* EOF */
