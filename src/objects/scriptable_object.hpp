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

#ifndef HEADER_WINDSTILLE_OBJECTS_SCRIPTABLE_OBJECT_HPP
#define HEADER_WINDSTILLE_OBJECTS_SCRIPTABLE_OBJECT_HPP

#include <string>

#include "engine/entity.hpp"
#include "sprite2d/sprite.hpp"

/** Can represent any generic object that the player may see or interact with that
    does not require collision physics or specialized C++ code. For example doors, trains,
    signs, objects you can pick up, and animations.

    TODO: should this provide optional collision detection such that you can use it to make
    crates etc? Or perhaps another object should derive from this which adds collision
    detection?
*/
class ScriptableObject : public Entity
{
private:
  Sprite sprite;
  Sprite light;
  Sprite highlight;
  float z_pos;

  //useable stuff
  std::string script_file;

  //movement stuff
  float target_x;
  float target_y;
  float target_speed;
  float acceleration;

  //flashing stuff
  float flash_speed;
  float flash_delta;

protected:
  void move(float delta);
  void flash();

public:
  ScriptableObject(const FileReader& reader);
  virtual ~ScriptableObject();

  void draw (SceneContext& sc);
  void update (float delta);
  void use();
  void move_to(float x, float y, float arg_target_speed, float arg_acceleration);
  void start_flash(float speed);
};

#endif

/* EOF */
