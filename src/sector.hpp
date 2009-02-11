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

#ifndef HEADER_WINDSTILLE_SECTOR_HPP
#define HEADER_WINDSTILLE_SECTOR_HPP

#include <string>
#include <vector>
#include "color.hpp"

class FileReader;
class GameObject;
class TileMap;
class Player;
class SceneContext;
class SpawnPoint;
class CollisionEngine;
class NavigationGraph;
class Entity;

/** */
class Sector
{
private: 
  static Sector* current_;  
public:
  static Sector* current() { return current_; }
  
private:
  std::string filename;
  std::string name;
  std::string music;
  std::string init_script;

  typedef std::vector<GameObject*> Objects;
  Objects objects;
  /** container for newly created GameObjects (they'll be added once per frame
   * in the update function */
  Objects new_objects;

  Color ambient_light;

  /** The TileMap with which the player interacts */
  TileMap* interactive_tilemap;
  TileMap* interactivebackground_tilemap;

  CollisionEngine* collision_engine;
  NavigationGraph* navigation_graph;

  Player* player;

  void parse_file(const std::string& filename);

  void commit_adds();
  void commit_removes();

  void expose_object_to_squirrel(GameObject* object);
  void remove_object_from_squirrel(GameObject* object);

public:
  Sector(const std::string& filename);
  ~Sector();

  const std::string& get_filename () const;

  void draw(SceneContext& gc);
  void update(float delta);

  /**
   * activates the sector. (Runs init script, changes music)
   */
  void activate();
  
  int get_width  () const;
  int get_height () const;

  /**
   * Set ambient light, ie. the light that illuminates the whole sector
   * all the time, to \a color
   */
  void  set_ambient_light(const Color& color);
  Color get_ambient_light() const;

  void add(GameObject*);
  void add_object(FileReader& reader);

  CollisionEngine* get_collision_engine() const { return collision_engine; }

  GameObject* get_object(const std::string& name) const;

  std::vector<GameObject*>* get_objects() { return &objects; }
  TileMap* get_tilemap() const { return interactive_tilemap; }
  TileMap* get_tilemap2() const { return interactivebackground_tilemap; }
  void     set_tilemap(TileMap* t);
  Player* get_player() const
  {
    return player;
  }
  
private:
  Sector (const Sector&);
  Sector& operator= (const Sector&);
};

#endif

/* EOF */
