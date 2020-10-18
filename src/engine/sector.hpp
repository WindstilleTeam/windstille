/*
**  Windstille - A Sci-Fi Action-Adventure Game
**  Copyright (C) 2005 Ingo Ruhnke <grumbel@gmail.com>
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
#include <memory>
#include <memory>

#include "display/color.hpp"
#include "engine/game_object_handle.hpp"
#include "util/currenton.hpp"
#include "util/pathname.hpp"

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

class Sector : public Currenton<Sector>
{
private:
  std::unique_ptr<CollisionEngine> collision_engine;
  std::unique_ptr<NavigationGraph> navigation_graph;
  std::unique_ptr<SceneGraph>      scene_graph;

  Pathname filename;
  std::string name;
  std::string music;
  std::string init_script;

  std::shared_ptr<SquirrelThread> vm;

  typedef std::vector<std::shared_ptr<GameObject> > Objects;
  Objects objects;
  /** container for newly created GameObjects (they'll be added once per frame
   * in the update function */
  Objects new_objects;

  RGBAf ambient_light;

public:
  /** The TileMap with which the player interacts */
  TileMap* interactive_tilemap;
  TileMap* interactivebackground_tilemap;

private:
  std::shared_ptr<Player> player;
  std::shared_ptr<Doll>   doll;

private:
  void parse_file(const Pathname& filename);

  void commit_adds();
  void commit_removes();

public:
  Sector(const Pathname& filename);
  ~Sector() override;

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
  void  set_ambient_light(const RGBAf& color);
  RGBAf get_ambient_light() const;

  void add(GameObjectHandle object);

  CollisionEngine* get_collision_engine() const { return collision_engine.get(); }
  SceneGraph& get_scene_graph() const { return *scene_graph; }
  NavigationGraph& get_navigation_graph() const { return *navigation_graph; }

  GameObject* get_object(const std::string& name) const;

  const std::vector<std::shared_ptr<GameObject> >& get_objects() { return objects; }
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
