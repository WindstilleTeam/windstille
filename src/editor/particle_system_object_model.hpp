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

#ifndef HEADER_WINDSTILLE_EDITOR_PARTICLE_SYSTEM_OBJECT_MODEL_HPP
#define HEADER_WINDSTILLE_EDITOR_PARTICLE_SYSTEM_OBJECT_MODEL_HPP

#include "editor/object_model.hpp"

class ParticleSystem;

class ParticleSystemObjectModel : public ObjectModel
{
private:
  typedef std::vector<ParticleSystem*> ParticleSystems;
  ParticleSystems systems;

public:
  ParticleSystemObjectModel(glm::vec2 const& pos);
  ~ParticleSystemObjectModel() override;

  void draw(wstdisplay::SceneContext& sc) override;
  void update(float delta) override;

  geom::frect get_bounding_box() const override;
  ObjectModelHandle clone() const override;

  void write(FileWriter& writer) const override;

  void add_to_scenegraph(wstdisplay::DrawableGroup& sg) override {}

private:
  ParticleSystemObjectModel(ParticleSystemObjectModel const&);
  ParticleSystemObjectModel& operator=(ParticleSystemObjectModel const&);
};

#endif

/* EOF */
