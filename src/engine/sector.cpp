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

#include <sstream>

#include "collision/collision_engine.hpp"
#include "engine/squirrel_thread.hpp"
#include "navigation/navigation_graph.hpp"
#include "scenegraph/navigation_graph_drawable.hpp"
#include "scenegraph/scene_graph.hpp"
#include "sound/sound_manager.hpp"
#include "tile/tile_map.hpp"

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
#include "objects/player.hpp"
#include "objects/scriptable_object.hpp"
#include "objects/shockwave.hpp"
#include "objects/spider_mine.hpp"
#include "objects/swarm.hpp"
#include "objects/test_object.hpp"
#include "objects/trigger.hpp"
#include "objects/vrdummy.hpp"

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
    player(0),
    id_table(),
    parent_table()
{
  if (debug) std::cout << "Creating new Sector" << std::endl;
  
  interactive_tilemap = 0;
  interactivebackground_tilemap = 0;

  parse_file(filename);
  if (interactive_tilemap)
  {
    // add interactive to collision engine
    collision_engine->add(new CollisionObject(interactive_tilemap));

    // Spawn the Player
    player = new Player();
    player->set_pos(Vector2f(300,200));
    add(player);
  }
  else
  {
    doll = new Doll();
    add(doll);
  }

  scene_graph->add_drawable(boost::shared_ptr<Drawable>(new NavigationGraphDrawable(navigation_graph.get())));
}

Sector::~Sector()
{
}

void
Sector::parse_file(const Pathname& filename_)
{
  if (debug) std::cout << "Sector:parse_file '" << filename_ << "'" << std::endl;
  
  FileReader reader = FileReader::parse(filename_);
  if(reader.get_name() != "windstille-sector") 
  {
    std::ostringstream msg;
    msg << "'" << filename_ << "' is not a windstille-sector file";
    throw std::runtime_error(msg.str());
  }
  else
  {
    int version = 1;
    if (!reader.get("version", version))
      std::cerr << "Warning no version specified in levelformat.\n";

    if (version > 3)
      std::cerr << "Warning: format version is newer than game.\n";

    reader.get("name",          name);
    reader.get("music",         music);
    reader.get("init-script",   init_script);
    reader.get("ambient-color", ambient_light);
  
    FileReader objects_reader;
    if (!reader.get("objects", objects_reader))
    {
      throw std::runtime_error("No objects specified");
    }

    std::vector<FileReader> objects_readers = objects_reader.get_sections();
    for(std::vector<FileReader>::iterator i = objects_readers.begin(); i != objects_readers.end(); ++i)
    {
      add_object(*i);
    }

    // Set the parents properly
    for(std::map<GameObject*, std::string>::iterator i = parent_table.begin(); i != parent_table.end(); ++i)
    {
      std::map<std::string, GameObject*>::iterator j = id_table.find(i->second);
      if (j == id_table.end())
      {
        std::cout << "Error: Couldn't resolve 'id': " << i->second << std::endl;
      }
      else
      {
        i->first->set_parent(j->second);
      }
    }

    FileReader navgraph_reader;
    if (reader.get("navigation", navgraph_reader))
    {
      navigation_graph->load(navgraph_reader);
    }

    if (debug) std::cout << "Finished parsing" << std::endl;
  }
}

void
Sector::add_object(FileReader& reader)
{
  GameObject* obj = 0;
  if(reader.get_name() == "tilemap") 
  {
    std::auto_ptr<TileMap> tilemap(new TileMap(reader));

    if (tilemap->get_name() == "interactive")
      interactive_tilemap = tilemap.get();
    else if (tilemap->get_name() == "interactivebackground")
      interactivebackground_tilemap = tilemap.get();

    obj = tilemap.release();
  }
  else if(reader.get_name() == "background")
  {
    // TODO
  }
  else if (reader.get_name() == "background-gradient")
  {
    obj = new BackgroundGradient(reader);
  }
  else if(reader.get_name() == "trigger")
  {
    obj = new Trigger(reader);
  }
  else if(reader.get_name() == "box")
  {
    obj = new Box(reader);
  }
  else if(reader.get_name() == "shockwave")
  {
    obj = new Shockwave(reader);
  }
  else if(reader.get_name() == "elevator")
  {
    obj = new Elevator(reader);
  }
  else if(reader.get_name() == "character")
  {    
    obj = new Character(reader);
  }
  else if(reader.get_name() == "spider-mine")
  {
    obj = new SpiderMine(reader);
  }
  else if(reader.get_name() == "hedgehog")
  {
    obj = new Hedgehog(reader);
  }
  else if(reader.get_name() == "test-object")
  {
    obj = new TestObject(reader);
  }
  else if (reader.get_name() == "nightvision")
  {
    obj = new Nightvision(reader);
  }
  else if (reader.get_name() == "particle-system")
  {
    // FIXME: disabled due to work on the editor: obj = new ParticleSystem(reader);
  }
  else if(reader.get_name() == "scriptable-object")
  {    
    obj = new ScriptableObject(reader);
  }
  else if(reader.get_name() == "decal")
  {    
    obj = new Decal(reader);
  }
  else if(reader.get_name() == "layer")
  {    
    obj = new Layer(reader);
  }
  else if (reader.get_name() == "vrdummy")
  {
    obj = new VRDummy(reader);
  }
  else if (reader.get_name() == "swarm")
  {
    obj = new Swarm(reader);
  }
  else if (reader.get_name() == "laserpointer")
  {
    obj = new LaserPointer();
  }
  else if (reader.get_name() == "liquid")
  {
    obj = new Liquid(reader);
  }
  else if (reader.get_name() == "particle-systems")
  {
    obj = new ParticleSystems(reader);
  }
  else 
  {
    std::cout << "Skipping unknown Object: " << reader.get_name() << "\n";
  }

  if (obj)
  {
    std::string id_str;
    if (reader.read("id", id_str))
    {
      id_table[id_str] = obj;
    }

    std::string parent_str;
    if (reader.read("parent", parent_str))
    {
      if (!parent_str.empty())
        parent_table[obj] = parent_str;
    }

    add(obj);
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
    Pathname path = get_directory();
    path.append_path(init_script);
    vm = ScriptManager::current()->run_script_file(path);
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

  //sc.color().draw(new SceneGraphDrawable(*scene_graph, sc.color().get_modelview()));
}

void Sector::commit_adds()
{
  if (!new_objects.empty())
  {
    // Add new game objects
    for(Objects::iterator i = new_objects.begin(); i != new_objects.end(); ++i) {
      objects.push_back(*i);
    }
    new_objects.clear();
  }
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
Sector::get_object(const std::string& name_) const
{
  for(Objects::const_iterator i = objects.begin(); i != objects.end(); ++i) 
    {
      if ((*i)->get_name() == name_)
        {
          return i->get();
        }
    }

  return 0;
}

int
Sector::get_width () const
{
  if (interactive_tilemap)
    return interactive_tilemap->get_width() * TILE_SIZE;
  else
    return 2560;
}

int
Sector::get_height () const
{
  if (interactive_tilemap)
    return interactive_tilemap->get_height() * TILE_SIZE;
  else
    return 1600;
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

Pathname
Sector::get_directory() const
{
  return filename.get_dirname();
}

void
Sector::call_script_function(const std::string& name_)
{
  if (!vm.get())
    {
      throw std::runtime_error("Sector::call_script_function(): Can't call function '" + name_ + "' without a init script");
    }
  else
    {
      if (!vm->is_idle())
        {
          throw std::runtime_error("Sector::call_script_function(): VM must be idle to call  '" + name_ + "'");
        }
      else
        {
          vm->call(name_);
        }
    }
}

/* EOF */
