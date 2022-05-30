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

#include "engine/object_factory.hpp"

#include "app/app.hpp"
#include "objects/background_gradient.hpp"
#include "objects/box.hpp"
#include "objects/character.hpp"
#include "objects/decal.hpp"
#include "objects/doll.hpp"
#include "objects/elevator.hpp"
#include "objects/hedgehog.hpp"
#include "objects/laser_pointer.hpp"
#include "objects/layer.hpp"
#include "objects/liquid.hpp"
#include "objects/nightvision.hpp"
#include "objects/particle_systems.hpp"
#include "objects/scriptable_object.hpp"
#include "objects/shockwave.hpp"
#include "objects/spider_mine.hpp"
#include "objects/swarm.hpp"
#include "objects/test_object.hpp"
#include "objects/trigger.hpp"
#include "objects/vrdummy.hpp"

namespace windstille {

GameObjectHandle
ObjectFactory::create(ReaderObject const& reader_obj)
{
  if (reader_obj.get_name() == "background-gradient")
  {
    return GameObjectHandle(new BackgroundGradient(reader_obj.get_mapping()));
  }
  else if(reader_obj.get_name() == "trigger")
  {
    return GameObjectHandle(new Trigger(reader_obj.get_mapping()));
  }
  else if(reader_obj.get_name() == "box")
  {
    return GameObjectHandle(new Box(reader_obj.get_mapping()));
  }
  else if(reader_obj.get_name() == "shockwave")
  {
    return GameObjectHandle(new Shockwave(reader_obj.get_mapping()));
  }
  else if(reader_obj.get_name() == "elevator")
  {
    return GameObjectHandle(new Elevator(reader_obj.get_mapping()));
  }
  else if(reader_obj.get_name() == "character")
  {
    return GameObjectHandle(new Character(reader_obj.get_mapping()));
  }
  else if(reader_obj.get_name() == "spider-mine")
  {
    return GameObjectHandle(new SpiderMine(reader_obj.get_mapping()));
  }
  else if(reader_obj.get_name() == "hedgehog")
  {
    return GameObjectHandle(new Hedgehog(reader_obj.get_mapping()));
  }
  else if(reader_obj.get_name() == "test-object")
  {
    return GameObjectHandle(new TestObject(reader_obj.get_mapping()));
  }
  else if (reader_obj.get_name() == "nightvision")
  {
    return GameObjectHandle(new Nightvision(reader_obj.get_mapping()));
  }
  else if (reader_obj.get_name() == "particle-system")
  {
    // FIXME: disabled due to work on the editor: return GameObjectHandle(new ParticleSystem(reader_obj.get_mapping()));
    return GameObjectHandle();
  }
  else if(reader_obj.get_name() == "scriptable-object")
  {
    return GameObjectHandle(new ScriptableObject(reader_obj.get_mapping()));
  }
  else if(reader_obj.get_name() == "decal")
  {
    return GameObjectHandle(new Decal(reader_obj.get_mapping()));
  }
  else if (reader_obj.get_name() == "vrdummy")
  {
    return GameObjectHandle(new VRDummy(reader_obj.get_mapping()));
  }
  else if (reader_obj.get_name() == "swarm")
  {
    return GameObjectHandle(new Swarm(reader_obj.get_mapping()));
  }
  else if (reader_obj.get_name() == "laserpointer")
  {
    return GameObjectHandle(new LaserPointer());
  }
  else if (reader_obj.get_name() == "liquid")
  {
    return GameObjectHandle(new Liquid(reader_obj.get_mapping()));
  }
  else if (reader_obj.get_name() == "particle-systems")
  {
    return GameObjectHandle(new ParticleSystems(reader_obj.get_mapping(), g_app.surface()));
  }
  else
  {
    return GameObjectHandle();
  }
}

} // namespace windstille

/* EOF */
