/*
**  Windstille - A Sci-Fi Action-Adventure Game
**  Copyright (C) 2009 Ingo Ruhnke <grumbel@gmx.de>
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

#include "engine/sector.hpp"
#include "particles/particle_system.hpp"
#include "scenegraph/scene_graph.hpp"
#include "scenegraph/particle_system_drawable.hpp"

ParticleSystems::ParticleSystems(const FileReader& reader) :
  m_systems(),
  m_drawables()
{
  std::string filename;
  Vector2f    pos;
  reader.get("name", filename);
  reader.get("pos",  pos);

  { // Load the ParticleSystems
    FileReader root_reader = FileReader::parse(Pathname(filename));
    if(root_reader.get_name() != "particle-systems") 
    {
      std::ostringstream msg;
      msg << "'" << filename << "' is not a particle-system file";
      throw std::runtime_error(msg.str());
    }

    std::vector<FileReader> sections = root_reader.get_sections();
    for(std::vector<FileReader>::iterator i = sections.begin(); i != sections.end(); ++i)
    { 
      if (i->get_name() == "particle-system")
      {
        boost::shared_ptr<ParticleSystem> particle_system(new ParticleSystem(*i));
        particle_system->set_pos(pos);
        m_systems.push_back(particle_system);
      }
    }
  }

  for(Systems::iterator i = m_systems.begin(); i != m_systems.end(); ++i)
  {
    boost::shared_ptr<ParticleSystemDrawable> drawable(new ParticleSystemDrawable(**i));

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

/* EOF */
