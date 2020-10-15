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

#ifndef HEADER_WINDSTILLE_PARTICLES_DRAWER_HPP
#define HEADER_WINDSTILLE_PARTICLES_DRAWER_HPP

class DrawingContext;
class GraphicsContext;
class ParticleSystem;

// FIXME: Rename this to ParticleDrawer or move it into a "namespace particles"
class Drawer
{
public:
  virtual ~Drawer() {}
  virtual void draw(GraphicsContext& gc, const ParticleSystem& psys) const =0;
};

#endif

/* EOF */
