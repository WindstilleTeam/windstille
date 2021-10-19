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

#include "particles/particle_system.hpp"
#include "editor/particle_system_object_model.hpp"

ParticleSystemObjectModel::ParticleSystemObjectModel(const glm::vec2& pos) :
  ObjectModel("ParticleSystem", pos),
  systems()
{
}

ParticleSystemObjectModel::~ParticleSystemObjectModel()
{
}

void
ParticleSystemObjectModel::draw(wstdisplay::SceneContext& /*sc*/)
{
}

void
ParticleSystemObjectModel::update(float delta)
{
  for(ParticleSystems::iterator i = systems.begin(); i != systems.end(); ++i)
    (*i)->update(delta);
}

geom::frect
ParticleSystemObjectModel::get_bounding_box() const
{
  return geom::frect(get_world_pos(), geom::fsize(128.0f, 128.0f));
}

ObjectModelHandle
ParticleSystemObjectModel::clone() const
{
  return ObjectModelHandle();
}

void
ParticleSystemObjectModel::write(FileWriter& /*writer*/) const
{
}

/* EOF */
