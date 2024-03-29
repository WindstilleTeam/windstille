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

#include "objects/particle_systems.hpp"

#include <sstream>
#include <stdexcept>

#include "app/app.hpp"
#include "engine/sector.hpp"
#include "particles/particle_system.hpp"
#include "particles/particle_system_drawable.hpp"
#include <wstdisplay/scenegraph/scene_graph.hpp>

namespace windstille {

ParticleSystems::ParticleSystems(ReaderMapping const& reader,
                                 wstdisplay::SurfaceManager& surface_manager) :
  m_systems(),
  m_drawables()
{
  std::string filename;
  glm::vec2    pos;
  reader.read("name", filename);
  reader.read("pos",  pos);

  { // Load the ParticleSystems
    ReaderDocument const& doc = ReaderDocument::from_file(Pathname(filename).get_sys_path());
    if (doc.get_name() != "particle-systems")
    {
      std::ostringstream msg;
      msg << "'" << filename << "' is not a particle-system file";
      throw std::runtime_error(msg.str());
    }

    ReaderCollection particlesys_collection;
    reader.read("particle-systems", particlesys_collection);
    for (ReaderObject const& item : particlesys_collection.get_objects()) {
      if (item.get_name() == "particle-system") {
        std::shared_ptr<ParticleSystem> particle_system(new ParticleSystem(item.get_mapping(), surface_manager));
        particle_system->set_pos(pos);
        m_systems.push_back(particle_system);
      }
    }
  }

  for(Systems::iterator i = m_systems.begin(); i != m_systems.end(); ++i)
  {
    std::shared_ptr<ParticleSystemDrawable> drawable(new ParticleSystemDrawable(**i));

    m_drawables.push_back(drawable);
    Sector::current()->get_scene_graph().add_drawable(drawable);
  }
}

void
ParticleSystems::update(float delta)
{
  for(Systems::iterator i = m_systems.begin(); i != m_systems.end(); ++i)
  {
    (*i)->update(delta);
  }
}

} // namespace windstille

/* EOF */
