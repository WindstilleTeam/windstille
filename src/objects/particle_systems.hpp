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

#ifndef HEADER_WINDSTILLE_OBJECTS_PARTICLE_SYSTEMS_HPP
#define HEADER_WINDSTILLE_OBJECTS_PARTICLE_SYSTEMS_HPP

#include <vector>
#include <memory>

#include <wstdisplay/fwd.hpp>

#include "engine/game_object.hpp"
#include "util/file_reader.hpp"

namespace windstille {

class ParticleSystem;
class ParticleSystemDrawable;

class ParticleSystems : public GameObject
{
private:
  typedef std::vector<std::shared_ptr<ParticleSystem> > Systems;
  typedef std::vector<std::shared_ptr<ParticleSystemDrawable> > Drawables;

  Systems   m_systems;
  Drawables m_drawables;

public:
  ParticleSystems(ReaderMapping const& reader, wstdisplay::SurfaceManager& surface_manager);

  void update (float delta) override;

private:
  ParticleSystems(ParticleSystems const&);
  ParticleSystems& operator=(ParticleSystems const&);
};

} // namespace windstille

#endif

/* EOF */
