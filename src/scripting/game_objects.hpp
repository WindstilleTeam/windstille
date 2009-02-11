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
#include "engine/game_object.hpp"
#include "objects/test_object.hpp"
#include "objects/player.hpp"
#include "objects/scriptable_object.hpp"
#include "ref.hpp"

typedef GameObject _GameObject;
typedef TestObject _TestObject;
typedef Player _Player;
typedef ScriptableObject _ScriptableObject;
typedef Entity _Entity;
#endif

namespace scripting
{

class GameObject
{
#ifndef SCRIPTING_API
protected:
  Ref<_GameObject> object;
  
public:
  GameObject(_GameObject* _object)
    : object(_object)
  {}
  virtual ~GameObject()
  {}
#endif
  
public:
  const std::string& get_name() const;
  void remove();
  void set_active(bool active);
  void set_parent(const std::string& name);
};

class TestObject : public GameObject
{
#ifndef SCRIPTING_API
public:
  TestObject(_TestObject* _object)
    : GameObject(_object)
  {}
  virtual ~TestObject()
  {}

  _TestObject* obj() const
  {
    return reinterpret_cast<_TestObject*> (object.get());
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
  Player(_Player* _player)
    : GameObject(_player)
  {}
  virtual ~Player()
  {}

  _Player* obj() const
  {
    return reinterpret_cast<_Player*> (object.get());
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
  ScriptableObject(_ScriptableObject* _object)
    : GameObject(_object)
  {}
  virtual ~ScriptableObject()
  {}

  _ScriptableObject* obj() const
  {
    return reinterpret_cast<_ScriptableObject*> (object.get());
  }
#endif

public:
  void move_to(float x, float y, float target_speed, float acceleration);
  void start_flash(float speed);
};
 
}

#endif

/* EOF */
