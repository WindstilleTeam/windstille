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
#include "engine/sector_builder.hpp"
#include "engine/squirrel_thread.hpp"
#include "navigation/navigation_graph.hpp"
#include "objects/doll.hpp"
#include "objects/player.hpp"
#include "scenegraph/navigation_graph_drawable.hpp"
#include "scenegraph/scene_graph.hpp"
#include "sound/sound_manager.hpp"
#include "tile/tile_map.hpp"

Sector::Sector(const Pathname& arg_filename) :
  collision_engine(new CollisionEngine()),
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
  player()
{
  SectorBuilder(arg_filename, *this);

  if (interactive_tilemap)
  {
    // add interactive to collision engine
    collision_engine->add(new CollisionObject(interactive_tilemap));

    // Spawn the Player
    player.reset(new Player());
    player->set_pos(Vector2f(300,200));
    add(player);
  }
  else
  {
    doll.reset(new Doll());
    add(doll);
  }

  scene_graph->add_drawable(boost::shared_ptr<Drawable>(new NavigationGraphDrawable(navigation_graph.get())));
}

Sector::~Sector()
{
}

void
Sector::activate()
{
  commit_adds();
  commit_removes();

  SoundManager::current()->play_music(Pathname(music, Pathname::kDataPath));

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
Sector::add(GameObjectHandle obj)
{
  new_objects.push_back(obj);

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
