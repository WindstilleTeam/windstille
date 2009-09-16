/*
**  Windstille - A Sci-Fi Action-Adventure Game
**  Copyright (C) 2000 Ingo Ruhnke <grumbel@gmx.de>
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

#ifndef HEADER_WINDSTILLE_ENGINE_GAME_OBJECT_HPP
#define HEADER_WINDSTILLE_ENGINE_GAME_OBJECT_HPP

#include <string>

#include "display/scene_context.hpp"
#include "engine/game_object_handle.hpp"
#include "util/file_reader.hpp"

class Sector;

/**
 * This is the base class for all in-game objects. The sector class keeps a list
 * of all GameObject and periodically calls draw() and update() for them.
 */
class GameObject
{
private:
  bool remove_flag;
 
protected:

  /**
   * name of the GameObject. Note: You should not change it anymore once the
   * object has been added to a Sector
   */
  std::string name;

  /** If a object is 'active = false' it will neither be drawn or updated */
  bool active;

public:
  GameObject() 
  : remove_flag(false), 
    name(),
    active(true)
  {}
  virtual ~GameObject() {}

  /**
   * Set the remove flag to true which will result in the object being removed
   * from the world till the next frame
   */
  void remove()
  {
    remove_flag = true;
  }
  /**
   * Returns the state of the remove flag
   */
  bool is_removable() const
  {
    return remove_flag;
  }

  /**
   * return the name of the object
   * The name is guaranteed to stay the same during the whole lifetime of
   * the object
   */
  const std::string& get_name() const
  {
    return name;
  }

  /**
   * Activate or Deactivate an object. A deactivated object is not updated
   * or drawn until it is activated again
   */
  void set_active(bool a) { active = a; }

  /**
   * Returns true if the object is active, false if it is inactive
   */
  bool is_active() const { return active; }

  /**
   * The object should draw itself when this function is called
   */
  virtual void draw (SceneContext& sc) {}

  /**
   * This function is called from time to time to give the object a chance to
   * update it's state. delta is the time that elapsed since the last
   * update call in seconds. It is possible that there are multiple objects
   * between 2 draw() calls or multiple draw() calls between 2 updates
   */
  virtual void update (float delta) {}
    
  virtual void set_parent(GameObjectHandle parent) {}

private:
  GameObject (const GameObject&);
  GameObject& operator= (const GameObject&);
};

#endif

/* EOF */
