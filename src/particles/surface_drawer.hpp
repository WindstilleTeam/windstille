/*  $Id$
**   __      __ __             ___        __   __ __   __
**  /  \    /  \__| ____    __| _/_______/  |_|__|  | |  |   ____
**  \   \/\/   /  |/    \  / __ |/  ___/\   __\  |  | |  | _/ __ \
**   \        /|  |   |  \/ /_/ |\___ \  |  | |  |  |_|  |_\  ___/
**    \__/\  / |__|___|  /\____ /____  > |__| |__|____/____/\___  >
**         \/          \/      \/    \/                         \/
**  Copyright (C) 2000,2005 Ingo Ruhnke <grumbel@gmx.de>
**
**  This program is free software; you can redistribute it and/or
**  modify it under the terms of the GNU General Public License
**  as published by the Free Software Foundation; either version 2
**  of the License, or (at your option) any later version.
**
**  This program is distributed in the hope that it will be useful,
**  but WITHOUT ANY WARRANTY; without even the implied warranty of
**  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
**  GNU General Public License for more details.
** 
**  You should have received a copy of the GNU General Public License
**  along with this program; if not, write to the Free Software
**  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA
**  02111-1307, USA.
*/

#ifndef HEADER_SURFACE_DRAWER_HXX
#define HEADER_SURFACE_DRAWER_HXX

#include "display/surface.hpp"
#include "ref.hpp"
#include "drawer.hpp"

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
};

#endif

/* EOF */
