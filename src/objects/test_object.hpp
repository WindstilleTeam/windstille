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

#ifndef HEADER_WINDSTILLE_OBJECTS_TEST_OBJECT_HPP
#define HEADER_WINDSTILLE_OBJECTS_TEST_OBJECT_HPP

#include <vector>

#include "engine/game_object.hpp"
#include "sprite3d/sprite3d.hpp"
#include "math/vector2f.hpp"

class TestObject : public GameObject
{
public:
  TestObject(const FileReader& reader);
  virtual ~TestObject();

  void draw(SceneContext& context);
  void update(float delta);

  void set_sprite(const std::string& filename);
  void set_action(const std::string& action);
  void set_vflip(bool vflip);
  void set_pos(const Vector2f& pos);
  void attach(const std::string& spritename,
              const std::string& attachement_point);

private:
  Sprite3D sprite;
  struct AttachedSprite {
    Sprite3D sprite;
    Sprite3D::PointID attachpoint;

    AttachedSprite()
      : sprite(),
        attachpoint()
    {}
  };
  std::vector<AttachedSprite> attached_sprites;
  Vector2f pos;
};

#endif

/* EOF */
