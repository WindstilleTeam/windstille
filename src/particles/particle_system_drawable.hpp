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

#ifndef HEADER_WINDSTILLE_SCENEGRAPH_PARTICLE_SYSTEM_DRAWABLE_HPP
#define HEADER_WINDSTILLE_SCENEGRAPH_PARTICLE_SYSTEM_DRAWABLE_HPP

#include <wstdisplay/scenegraph/drawable.hpp>

namespace windstille {

class ParticleSystem;

class ParticleSystemDrawable : public wstdisplay::Drawable
{
private:
  ParticleSystem const& m_particle_system;

public:
  ParticleSystemDrawable(ParticleSystem const& particle_system);

  void render(wstdisplay::GraphicsContext& gc, unsigned int mask) override;

private:
  ParticleSystemDrawable(ParticleSystemDrawable const&);
  ParticleSystemDrawable& operator=(ParticleSystemDrawable const&);
};

} // namespace windstille

#endif

/* EOF */
