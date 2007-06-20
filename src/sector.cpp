//  $Id$
//
//  Windstille - A Jump'n Shoot Game
//  Copyright (C) 2005 Ingo Ruhnke <grumbel@gmx.de>
//
//  This program is free software; you can redistribute it and/or
//  modify it under the terms of the GNU General Public License
//  as published by the Free Software Foundation; either version 2
//  of the License, or (at your option) any later version.
//
//  This program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
//
//  You should have received a copy of the GNU General Public License
//  along with this program; if not, write to the Free Software
//  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.

#include <iostream>
#include <sstream>
#include <stdexcept>
#include "lisp/properties.hpp"
#include "lisp/parser.hpp"
#include "lisp_getters.hpp"
#include "sexpr_file_reader.hpp"
#include "globals.hpp"
#include "display/scene_context.hpp"
#include "objects/background_gradient.hpp"
#include "tile_map.hpp"
#include "game_object.hpp"
#include "player.hpp"
#include "trigger.hpp"
#include "objects/test_object.hpp"
#include "sector.hpp"
#include "objects/vrdummy.hpp"
#include "sound/sound_manager.hpp"
#include "script_manager.hpp"
#include "collision/collision_engine.hpp"
#include "particles/particle_system.hpp"
#include "objects/test_object.hpp"
#include "elevator.hpp"
#include "objects/nightvision.hpp"
#include "character.hpp"
#include "laser_pointer.hpp"
#include "objects/swarm.hpp"
#include "liquid.hpp"
#include "objects/shockwave.hpp"
#include "objects/hedgehog.hpp"
#include "objects/spider_mine.hpp"
#include "box.hpp"
#include "scriptable_object.hpp"
#include "navigation/navigation_graph.hpp"
#include "scripting/squirrel_error.hpp"

// The table (works like a namespace here) where the game objects will appear
#define OBJECTS_TABLE "objects"

Sector* Sector::current_ = 0;

Sector::Sector(const std::string& arg_filename)
  : filename(arg_filename),
    player(0)    
{
  // make sure squirrel has an "objects" table
  script_manager->run_script(
      "if(! (\"" OBJECTS_TABLE "\" in this)) {"
      "  " OBJECTS_TABLE " <- {};"
      "}", "");
  
  if (debug) std::cout << "Creating new Sector" << std::endl;
  collision_engine = new CollisionEngine();
  navigation_graph = new NavigationGraph();

  current_ = this;
  interactive_tilemap = 0;
  interactivebackground_tilemap = 0;

  parse_file(filename);
  if (!interactive_tilemap)
    throw std::runtime_error("No interactive-tilemap available");

  // add interactive to collision engine
  collision_engine->add(new CollisionObject(interactive_tilemap));

  // Spawn the Player
  if(!player) {
    player = new Player();
    add(player);
  }
  player->set_pos(Vector(300,200));
}

Sector::~Sector()
{
  for(Objects::iterator i = objects.begin(); i != objects.end(); ++i)
    (*i)->unref();
  for(Objects::iterator i = new_objects.begin(); i != new_objects.end(); ++i)
    (*i)->unref();

  delete navigation_graph;
  delete collision_engine;
}

void
Sector::parse_file(const std::string& filename)
{
  if (debug) std::cout << "Sector:parse_file '" << filename << "'" << std::endl;
  using namespace lisp;
  
  SExprFileReader reader(filename);
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

  reader.print_unused_warnings("sector");
  if (debug) std::cout << "Finished parsing" << std::endl;
}

void
Sector::add_object(FileReader& reader)
{
  if(reader.get_name() == "tilemap") {
    TileMap* tilemap = new TileMap(reader);
    add(tilemap);
    if (tilemap->get_name() == "interactive")
      interactive_tilemap = tilemap;
    else if (tilemap->get_name() == "interactivebackground")
      interactivebackground_tilemap = tilemap;
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
    add(new ParticleSystem(reader));
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

  sound_manager->play_music(music);
  if (init_script != "")
    script_manager->run_script_file(init_script);
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

  for(Objects::iterator i = objects.begin(); i != objects.end(); ++i) {
    if ((*i)->is_active())
      (*i)->update(delta);
  }
  commit_removes();
}

void
Sector::commit_removes()
{
  // remove objects
  for(Objects::iterator i = objects.begin(); i != objects.end(); ) {
    GameObject* object = *i;
    if(object->is_removable()) {
      if(object->get_name() != "") {
        remove_object_from_squirrel(object);
      }
      object->unref();

      i = objects.erase(i);
      continue;
    }

    ++i;
  }
}

void
Sector::add(GameObject* obj)
{
  new_objects.push_back(obj);
  obj->ref();
  if(obj->get_name() != "") {
    expose_object_to_squirrel(obj);
  }
}

void
Sector::remove_object_from_squirrel(GameObject* object)
{
  using namespace Scripting;

  // get objects table
  HSQUIRRELVM v = script_manager->get_vm();
  sq_pushroottable(v);
  sq_pushstring(v, OBJECTS_TABLE, -1);
  if(SQ_FAILED(sq_get(v, -2)))
  {
    std::ostringstream msg;
    msg << "Couldn't get objects table '" << OBJECTS_TABLE << "'";
    throw SquirrelError(v, msg.str());
  }

  // remove object from table
  sq_pushstring(v, object->get_name().c_str(), object->get_name().size());
  if(SQ_FAILED(sq_deleteslot(v, -2, SQFalse) < 0)) {
    std::ostringstream msg;
    msg << "Couldn't remove squirrel object for '" << object->get_name()
        << "'";
    throw SquirrelError(v, msg.str());
  }
  
  // pop objects and root table
  sq_pop(v, 2);
}

// tries to find out the "real" class of an gameobject by some dynamic casting
// and creates a matching squirrel instance for that object
static inline void create_squirrel_instance(HSQUIRRELVM v, GameObject* object)
{
  ScriptableObject* script_obj = dynamic_cast<ScriptableObject*> (object);
  if(script_obj) {
    create_squirrel_instance(v, new Scripting::ScriptableObject(script_obj),
                             true);
    return;
  }
  
  TestObject* tobj = dynamic_cast<TestObject*> (object);
  if(tobj) {
    create_squirrel_instance(v, new Scripting::TestObject(tobj), true);
    return;
  }                                                                             

  Player* player = dynamic_cast<Player*> (object);
  if(player) {
    create_squirrel_instance(v, new Scripting::Player(player), true);
    return;
  }

  create_squirrel_instance(v, new Scripting::GameObject(object), true);
}

void
Sector::expose_object_to_squirrel(GameObject* object)
{
  using namespace Scripting;

  // get objects table
  HSQUIRRELVM v = script_manager->get_vm();
  sq_pushroottable(v);
  sq_pushstring(v, OBJECTS_TABLE, -1);
  if(SQ_FAILED(sq_get(v, -2)))
  {
    std::ostringstream msg;
    msg << "Couldn't get objects table '" << OBJECTS_TABLE << "'";
    throw SquirrelError(v, msg.str());
  }
  
  // create squirrel instance and register in table
  sq_pushstring(v, object->get_name().c_str(), object->get_name().size());
  create_squirrel_instance(v, object);
  if(SQ_FAILED(sq_createslot(v, -3)))
  {
    std::ostringstream msg;
    msg << "Couldn't register object in objects taböe";
    throw SquirrelError(v, msg.str());
  }

  // pop roottable and objects table
  sq_pop(v, 2);
}

GameObject*
Sector::get_object(const std::string& name) const
{
  for(Objects::const_iterator i = objects.begin(); i != objects.end(); ++i) 
    {
      if ((*i)->get_name() == name)
        {
          return *i;
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

const std::string&
Sector::get_filename () const
{
  return filename;
}

/* EOF */
