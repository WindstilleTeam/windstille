/*
**  Windstille - A Sci-Fi Action-Adventure Game
**  Copyright (C) 2005 Matthias Braun <matze@braunis.de>
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

#ifndef HEADER_WINDSTILLE_SCRIPTING_GAME_OBJECTS_HPP
#define HEADER_WINDSTILLE_SCRIPTING_GAME_OBJECTS_HPP

#ifndef SCRIPTING_API
#  include "engine/game_object.hpp"
#  include "objects/test_object.hpp"
#  include "objects/player.hpp"
#  include "objects/scriptable_object.hpp"
#endif

namespace Scripting {

class GameObject
{
#ifndef SCRIPTING_API
protected:
  std::weak_ptr< ::GameObject> object;

public:
  GameObject(std::shared_ptr< ::GameObject > _object)
    : object(_object)
  {}
  virtual ~GameObject()
  {}
#endif

public:
  const std::string& get_name() const;
  void remove();
  void set_active(bool active);
  bool is_active() const;
  void set_parent(const std::string& name);
};

class TestObject : public GameObject
{
#ifndef SCRIPTING_API
public:
  TestObject(std::shared_ptr< ::GameObject > _object)
    : GameObject(_object)
  {}
  ~TestObject() override
  {}

  ::TestObject* obj() const
  {
    return reinterpret_cast< ::TestObject* >(object.lock().get());
  }
#endif

public:
  void set_sprite(const std::string& filename);
  void set_action(const std::string& action);
  void set_pos(float x, float y);
  void set_vflip(bool vflip);
  void attach(const std::string& spritename,
              const std::string& attachement_point);
};

class Player : public GameObject
{
#ifndef SCRIPTING_API
public:
  Player(std::shared_ptr< ::GameObject > _player)
    : GameObject(_player)
  {}
  ~Player() override
  {}

  ::Player* obj() const
  {
    return reinterpret_cast< ::Player*> (object.lock().get());
  }
#endif

public:
  void start_listening();
  void stop_listening();
};

class ScriptableObject : public GameObject
{
#ifndef SCRIPTING_API
public:
  ScriptableObject(std::shared_ptr< ::GameObject > _object)
    : GameObject(_object)
  {}
  ~ScriptableObject() override
  {}

  ::ScriptableObject* obj() const
  {
    return reinterpret_cast< ::ScriptableObject*> (object.lock().get());
  }
#endif

public:
  void move_to(float x, float y, float target_speed, float acceleration);
  void start_flash(float speed);
};

} // namespace Scripting

#endif

/* EOF */
