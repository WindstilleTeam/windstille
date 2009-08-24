/*
**  Windstille - A Sci-Fi Action-Adventure Game
**  Copyright (C) 2005 Ingo Ruhnke <grumbel@gmx.de>
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

#include "engine/sector.hpp"

#include "objects/background_gradient.hpp"
#include "tile/tile_map.hpp"
#include "objects/player.hpp"
#include "objects/trigger.hpp"
#include "objects/test_object.hpp"
#include "objects/vrdummy.hpp"
#include "sound/sound_manager.hpp"
#include "collision/collision_engine.hpp"
#include "objects/elevator.hpp"
#include "objects/nightvision.hpp"
#include "objects/character.hpp"
#include "objects/laser_pointer.hpp"
#include "objects/swarm.hpp"
#include "objects/liquid.hpp"
#include "objects/shockwave.hpp"
#include "objects/hedgehog.hpp"
#include "objects/spider_mine.hpp"
#include "objects/box.hpp"
#include "objects/scriptable_object.hpp"
#include "navigation/navigation_graph.hpp"
#include "scenegraph/scene_graph.hpp"
#include "engine/squirrel_thread.hpp"

Sector::Sector(const Pathname& arg_filename)
  : collision_engine(new CollisionEngine()),
    navigation_graph(new NavigationGraph()),
    scene_graph(new SceneGraph()),
    filename(arg_filename),
    name(),
    music(),
    init_script(),
    vm(),
    objects(),
    new_objects(),
    ambient_light(),
    interactive_tilemap(0),
    interactivebackground_tilemap(0),
    player(0)
{
  //current_ = this;

  if (debug) std::cout << "Creating new Sector" << std::endl;
  
  interactive_tilemap = 0;
  interactivebackground_tilemap = 0;

  parse_file(filename);
  if (!interactive_tilemap)
    throw std::runtime_error("No interactive-tilemap available");

  // add interactive to collision engine
  collision_engine->add(new CollisionObject(interactive_tilemap));

  // Spawn the Player
  player = new Player();
  add(player);

  player->set_pos(Vector2f(300,200));
}

Sector::~Sector()
{
}

void
Sector::parse_file(const Pathname& filename)
{
  if (debug) std::cout << "Sector:parse_file '" << filename << "'" << std::endl;
  
  FileReader reader = FileReader::parse(filename);
  if(reader.get_name() != "windstille-sector") {
    std::ostringstream msg;
    msg << "'" << filename << "' is not a windstille-sector file";
    throw std::runtime_error(msg.str());
  }
  
  int version = 1;
  if(!reader.get("version", version))
    std::cerr << "Warning no version specified in levelformat.\n";
  if(version > 1)
    std::cerr << "Warning: format version is newer than game.\n";

  reader.get("name",          name);
  reader.get("music",         music);
  reader.get("init-script",   init_script);
  reader.get("ambient-color", ambient_light);
  
  FileReader objects_reader;
  if(reader.get("objects", objects_reader) == false)
    throw std::runtime_error("No objects specified");

  std::vector<FileReader> objects_readers = objects_reader.get_sections();
  for(std::vector<FileReader>::iterator i = objects_readers.begin(); i != objects_readers.end(); ++i)
    {
      add_object(*i);
    }

  if (debug) std::cout << "Finished parsing" << std::endl;
}

void
Sector::add_object(FileReader& reader)
{
  if(reader.get_name() == "tilemap") {
    std::auto_ptr<TileMap> tilemap(new TileMap(reader));

    if (tilemap->get_name() == "interactive")
      interactive_tilemap = tilemap.get();
    else if (tilemap->get_name() == "interactivebackground")
      interactivebackground_tilemap = tilemap.get();

    add(tilemap.release());
  } else if(reader.get_name() == "background") {
    // TODO
  } else if (reader.get_name() == "background-gradient") {
    add(new BackgroundGradient(reader));
  } else if(reader.get_name() == "trigger") {
    add(new Trigger(reader));
  } else if(reader.get_name() == "box") {
    add(new Box(reader));
  } else if(reader.get_name() == "shockwave") {
    add(new Shockwave(reader));
  } else if(reader.get_name() == "elevator") {
    add(new Elevator(reader));
  } else if(reader.get_name() == "character") {    
    add(new Character(reader));
  } else if(reader.get_name() == "spider-mine") {
    add(new SpiderMine(reader));
  } else if(reader.get_name() == "hedgehog") {
    add(new Hedgehog(reader));
  } else if(reader.get_name() == "test-object") {
    add(new TestObject(reader));
  } else if (reader.get_name() == "nightvision") {
    add(new Nightvision(reader));
  } else if (reader.get_name() == "particle-system") {
    // FIXME: disabled due to work on the editor: add(new ParticleSystem(reader));
  } else if(reader.get_name() == "scriptable-object") {    
    add(new ScriptableObject(reader));
  } else if (reader.get_name() == "vrdummy") {
    add(new VRDummy(reader));
  } else if (reader.get_name() == "swarm") {
    add(new Swarm(reader));
  } else if (reader.get_name() == "laserpointer") {
    add(new LaserPointer());
  } else if (reader.get_name() == "liquid") {
    add(new Liquid(reader));
  } else {
    std::cout << "Skipping unknown Object: " << reader.get_name() << "\n";
  }
}

void
Sector::activate()
{
  commit_adds();
  commit_removes();

  SoundManager::current()->play_music(music);

  if (!init_script.empty())
    {
      vm = ScriptManager::current()->run_script_file(get_directory() + init_script);
    }
}

void
Sector::draw(SceneContext& sc)
{
  sc.light().fill_screen(ambient_light);

  for(Objects::iterator i = objects.begin(); i != objects.end(); ++i)
    {
      if ((*i)->is_active())
        (*i)->draw(sc);
    }
}

void Sector::commit_adds()
{
  // Add new game objects
  for(Objects::iterator i = new_objects.begin(); i != new_objects.end(); ++i) {
    objects.push_back(*i);
  }
  new_objects.clear();
}

void Sector::update(float delta)
{
  commit_adds();

  collision_engine->update(delta);

  for(Objects::iterator i = objects.begin(); i != objects.end(); ++i) 
    {
      if ((*i)->is_active())
        (*i)->update(delta);
    }

  commit_removes();
}

void
Sector::commit_removes()
{
  // remove objects
  for(Objects::iterator i = objects.begin(); i != objects.end(); ) 
    {
      boost::shared_ptr<GameObject>& object = *i;

      if(object->is_removable()) 
        {
          if(object->get_name() != "") 
            {
              ScriptManager::current()->remove_object_from_squirrel(object);
            }

          i = objects.erase(i);
          continue;
        }

      ++i;
    }
}

void
Sector::add(GameObject* obj)
{
  new_objects.push_back(boost::shared_ptr<GameObject>(obj));

  if(obj->get_name() != "") 
    {
      ScriptManager::current()->expose_object_to_squirrel(new_objects.back());
    }
}

GameObject*
Sector::get_object(const std::string& name) const
{
  for(Objects::const_iterator i = objects.begin(); i != objects.end(); ++i) 
    {
      if ((*i)->get_name() == name)
        {
          return i->get();
        }
    }

  return 0;
}

int
Sector::get_width () const
{
  return interactive_tilemap->get_width() * TILE_SIZE;
}

int
Sector::get_height () const
{
  return interactive_tilemap->get_height() * TILE_SIZE;
}

void
Sector::set_tilemap(TileMap* t)
{
  interactive_tilemap = t;
}

void
Sector::set_ambient_light(const Color& color)
{
  ambient_light = color;
}

Color
Sector::get_ambient_light() const
{
  return ambient_light;
}

Pathname
Sector::get_filename () const
{
  return filename;
}

std::string
Sector::get_directory() const
{
  // FIXME: Not pretty
  std::string directory = filename.get_physfs_path();
  directory.erase(directory.rfind('/')+1);
  return directory;
}

void
Sector::call_script_function(const std::string& name)
{
  if (!vm.get())
    {
      throw std::runtime_error("Sector::call_script_function(): Can't call function '" + name + "' without a init script");
    }
  else
    {
      if (!vm->is_idle())
        {
          throw std::runtime_error("Sector::call_script_function(): VM must be idle to call  '" + name + "'");
        }
      else
        {
          vm->call(name);
        }
    }
}

/* EOF */
