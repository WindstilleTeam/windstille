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

#ifndef HEADER_WINDSTILLE_ENGINE_SECTOR_HPP
#define HEADER_WINDSTILLE_ENGINE_SECTOR_HPP

#include <string>
#include <vector>
#include <boost/shared_ptr.hpp>
#include <boost/scoped_ptr.hpp>

#include "util/pathname.hpp"
#include "util/currenton.hpp"
#include "display/color.hpp"

class CollisionEngine;
class Entity;
class FileReader;
class GameObject;
class NavigationGraph;
class Player;
class SceneContext;
class SpawnPoint;
class SquirrelThread;
class TileMap;
class SceneGraph;
class Doll;

/** */
class Sector : public Currenton<Sector>
{ 
private:
  boost::scoped_ptr<CollisionEngine> collision_engine;
  boost::scoped_ptr<NavigationGraph> navigation_graph;
  boost::scoped_ptr<SceneGraph>      scene_graph;

  Pathname filename;
  std::string name;
  std::string music;
  std::string init_script;
  
  boost::shared_ptr<SquirrelThread> vm;

  typedef std::vector<boost::shared_ptr<GameObject> > Objects;
  Objects objects;
  /** container for newly created GameObjects (they'll be added once per frame
   * in the update function */
  Objects new_objects;

  Color ambient_light;

  /** The TileMap with which the player interacts */
  TileMap* interactive_tilemap;
  TileMap* interactivebackground_tilemap;

  Player* player;
  Doll*   doll;

  std::map<std::string, GameObject*> id_table;
  std::map<GameObject*, std::string> parent_table;

private:
  void parse_file(const Pathname& filename);

  void commit_adds();
  void commit_removes();

public:
  Sector(const Pathname& filename);
  ~Sector();

  Pathname get_filename() const;
  Pathname get_directory() const;

  void draw(SceneContext& gc);
  void update(float delta);

  /**
   * activates the sector. (Runs init script, changes music)
   */
  void activate();
  
  int get_width() const;
  int get_height() const;

  /**
   * Set ambient light, ie. the light that illuminates the whole sector
   * all the time, to \a color
   */
  void  set_ambient_light(const Color& color);
  Color get_ambient_light() const;

  void add(GameObject*);
  void add_object(FileReader& reader);

  CollisionEngine* get_collision_engine() const { return collision_engine.get(); }
  SceneGraph& get_scene_graph() const { return *scene_graph; }
  NavigationGraph& get_navigation_graph() const { return *navigation_graph; }

  GameObject* get_object(const std::string& name) const;

  const std::vector<boost::shared_ptr<GameObject> >& get_objects() { return objects; }
  TileMap* get_tilemap() const { return interactive_tilemap; }
  TileMap* get_tilemap2() const { return interactivebackground_tilemap; }
  void     set_tilemap(TileMap* t);

  void call_script_function(const std::string& name);
  
private:
  Sector (const Sector&);
  Sector& operator= (const Sector&);
};

#endif

/* EOF */
