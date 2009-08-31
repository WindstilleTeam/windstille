/*
**  Windstille - A Sci-Fi Action-Adventure Game
**  Copyright (C) 2000,2005 Ingo Ruhnke <grumbel@gmx.de>
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

#include "display/surface.hpp"
#include "particles/drawer.hpp"

class SurfaceDrawer : public Drawer
{
private:
  Surface surface;
  GLenum blendfunc_src;
  GLenum blendfunc_dest;
  
public:
  SurfaceDrawer(FileReader& props);
  SurfaceDrawer(Surface surface);
  virtual ~SurfaceDrawer();
  
  void set_texture(Surface surface);
  void set_blendfuncs(GLenum blendfunc_src, GLenum blendfunc_dst);

  void draw(DrawingContext& sc, ParticleSystem& psys);
  void draw(const ParticleSystem& psys) const;
};

#endif

/* EOF */
