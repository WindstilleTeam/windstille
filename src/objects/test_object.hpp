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
#include <glm/glm.hpp>

namespace windstille {

class TestObject : public GameObject
{
public:
  TestObject(ReaderMapping const& reader);
  ~TestObject() override;

  void draw(wstdisplay::SceneContext& context) override;
  void update(float delta) override;

  void set_sprite(std::string const& filename);
  void set_action(std::string const& action);
  void set_vflip(bool vflip);
  void set_pos(glm::vec2 const& pos);
  void attach(std::string const& spritename,
              std::string const& attachement_point);

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
  glm::vec2 pos;
};

} // namespace windstille

#endif

/* EOF */
