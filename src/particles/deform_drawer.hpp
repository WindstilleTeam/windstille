/*
**  Windstille - A Sci-Fi Action-Adventure Game
**  Copyright (C) 2005 Ingo Ruhnke <grumbel@gmail.com>
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

#ifndef HEADER_WINDSTILLE_PARTICLES_DEFORM_DRAWER_HPP
#define HEADER_WINDSTILLE_PARTICLES_DEFORM_DRAWER_HPP

#include "particles/drawer.hpp"
#include "util/file_reader.hpp"

namespace windstille {

class ParticleSystem;

/** Meant to draw the particles into a deform map that distorts the
    screen, for heat effects from fire and such. The effect was never
    finished and doesn't draw anything. */
class DeformDrawer : public Drawer
{
public:
  explicit DeformDrawer(ReaderMapping const& props);
  ~DeformDrawer() override;

  void draw(wstdisplay::Canvas& canvas, ParticleSystem const& psys) const override;

private:
  DeformDrawer (DeformDrawer const&);
  DeformDrawer& operator= (DeformDrawer const&);
};

} // namespace windstille

#endif

/* EOF */
