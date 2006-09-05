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
#include <config.h>

#include <iostream>
#include <sstream>
#include <stdexcept>
#include "lisp/properties.hpp"
#include "lisp/parser.hpp"
#include "lisp_getters.hpp"
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
#include "spawnpoint.hpp"
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
#include "scripting/squirrel_error.hpp"

// The table (works like a namespace here) where the game objects will appear
#define OBJECTS_TABLE "objects"

Sector* Sector::current_ = 0;

const std::string&
Sector::get_filename () const
{
  return filename;
}

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

  current_ = this;
  interactive_tilemap = 0;
  interactivebackground_tilemap = 0;

  parse_file(filename);
  if (!interactive_tilemap)
    throw std::runtime_error("No interactive-tilemap available");

  // add interactive to collision engine
  collision_engine->add(new CollisionObject(interactive_tilemap));
}

Sector::~Sector()
{
  for(SpawnPoints::iterator i = spawn_points.begin();
      i != spawn_points.end(); ++i)
    delete *i;                                         
  for(Objects::iterator i = objects.begin(); i != objects.end(); ++i)
    (*i)->unref();
  for(Objects::iterator i = new_objects.begin(); i != new_objects.end(); ++i)
    (*i)->unref();

  delete collision_engine;
}

void
Sector::parse_file(const std::string& filename)
{
  if (debug) std::cout << "Sector:parse_file '" << filename << "'" << std::endl;
  using namespace lisp;
  
  std::auto_ptr<Lisp> root(Parser::parse(filename));
  Properties rootp(root.get());

  const Lisp* sector = 0;
  if(!rootp.get("windstille-sector", sector)) {
    std::ostringstream msg;
    msg << "'" << filename << "' is not a windstille-sector file";
    throw std::runtime_error(msg.str());
  }
  rootp.print_unused_warnings("sector");
  
  Properties props(sector);

  int version = 1;
  if(!props.get("version", version))
    std::cerr << "Warning no version specified in levelformat.\n";
  if(version > 1)
    std::cerr << "Warning: format version is newer than game.\n";
  props.get("name", name);
  props.get("music", music);
  props.get("init-script", init_script);
  props.get("ambient-color", ambient_light);
  
  PropertyIterator<const Lisp*> iter;
  props.get_iter("spawnpoint", iter);
  while(iter.next()) {
    spawn_points.push_back(new SpawnPoint(*iter));
  }

  const Lisp* objects = 0;
  if(props.get("objects", objects) == false)
    throw std::runtime_error("No objects specified");
  Properties pobjects(objects);
  iter = pobjects.get_iter();
  while(iter.next()) {
    add_object(iter.item(), *iter);
  }

  props.print_unused_warnings("sector");
  if (debug) std::cout << "Finished parsing" << std::endl;
}

void
Sector::add_object(const std::string& name, const lisp::Lisp* lisp)
{
  lisp::Properties props(lisp);

  if(name == "tilemap") {
    TileMap* tilemap = new TileMap(props);
    add(tilemap);
    if (tilemap->get_name() == "interactive")
      interactive_tilemap = tilemap;
    else if (tilemap->get_name() == "interactivebackground")
      interactivebackground_tilemap = tilemap;
  } else if(name == "background") {
    // TODO
  } else if (name == "background-gradient") {
    add(new BackgroundGradient(props));
  } else if(name == "trigger") {
    add(new Trigger(props));
  } else if(name == "box") {
    add(new Box(props));
  } else if(name == "shockwave") {
    add(new Shockwave(props));
  } else if(name == "elevator") {
    add(new Elevator(props));
  } else if(name == "character") {    
    add(new Character(props));
  } else if(name == "spider-mine") {
    add(new SpiderMine(props));
  } else if(name == "hedgehog") {
    add(new Hedgehog(props));
  } else if(name == "test-object") {
    add(new TestObject(props));
  } else if (name == "nightvision") {
    add(new Nightvision(props));
  } else if (name == "particle-system") {
    add(new ParticleSystem(props));
  } else if(name == "scriptable-object") {    
    add(new ScriptableObject(props));
  } else if (name == "vrdummy") {
    add(new VRDummy(props));
  } else if (name == "swarm") {
    add(new Swarm(props));
  } else if (name == "laserpointer") {
    add(new LaserPointer());
  } else if (name == "liquid") {
    add(new Liquid(props));
  } else {
    std::cout << "Skipping unknown Object: " << name << "\n";
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
Sector::spawn_player(const std::string& spawnpoint)
{
  const SpawnPoint* result = 0;
  for(SpawnPoints::iterator i = spawn_points.begin();
      i != spawn_points.end(); ++i) {
    const SpawnPoint* sp = *i;
    if(sp->name == spawnpoint) {
      result = sp;
      break;
    }
  }

  Vector spawnpos(320, 200);
  if(result == 0) {
    if(spawnpoint != "default") {
      std::cerr << "SpawnPoint '" << spawnpoint << "' not found.\n";
      spawn_player("default");
      return;
    }
  } else {
    spawnpos = result->pos;
  }

  if(!player) {
    player = new Player();
    add(player);
  }
  player->set_pos(spawnpos);
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

/* EOF */
