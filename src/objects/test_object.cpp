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

#include <iostream>
#include <stdexcept>

#include "app/app.hpp"
#include "objects/test_object.hpp"
#include "util/pathname.hpp"
#include "sprite3d/manager.hpp"

TestObject::TestObject(ReaderMapping const& props) :
  sprite(),
  attached_sprites(),
  pos()
{
  pos = glm::vec2(0, 0);
  std::string spritename;

  props.read("sprite", spritename);
  props.read("pos", pos);
  props.read("name", name);

  if(spritename == "")
    throw std::runtime_error("No sprite name specified in TestObject");
  sprite = g_app.sprite3d().create(Pathname(spritename));
}

TestObject::~TestObject()
{
}

void
TestObject::draw(wstdisplay::SceneContext& sc)
{
  sprite.draw(sc.color(), pos, 100);
  for(std::vector<AttachedSprite>::iterator i = attached_sprites.begin();
      i != attached_sprites.end(); ++i) {
    sc.push_modelview();
    sc.translate(pos.x, pos.y);
    sc.mult_modelview(sprite.get_attachment_point_matrix(i->attachpoint));

    i->sprite.draw(sc.color(), glm::vec2(0, 0), 100);
    sc.pop_modelview();
  }
}

void
TestObject::update(float delta)
{
  sprite.update(delta);
  for(std::vector<AttachedSprite>::iterator i = attached_sprites.begin();
      i != attached_sprites.end(); ++i) {
    i->sprite.update(delta);
  }
}

void
TestObject::set_sprite(const std::string& filename)
{
  try {
    sprite = g_app.sprite3d().create(Pathname(filename));
  } catch(std::exception& e) {
    std::cerr << "Couldn't change sprite to '" << filename << "': "
              << e.what() << "\n";
  }
}

void
TestObject::set_action(const std::string& action)
{
  try {
    sprite.set_action(action);
  } catch(std::exception& e) {
    std::cerr << "Couldn't change action to '" << action << "': "
              << e.what() << "\n";
  }
}

void
TestObject::set_pos(const glm::vec2& pos_)
{
  pos = pos_;
}

void
TestObject::set_vflip(bool vflip)
{
  sprite.set_rot(vflip);
}

void
TestObject::attach(const std::string& spritename,
                   const std::string& attachement_point)
{
  AttachedSprite asprite;
  asprite.sprite = g_app.sprite3d().create(Pathname(spritename));
  asprite.attachpoint = sprite.get_attachment_point_id(attachement_point);
  attached_sprites.push_back(asprite);
}

/* EOF */
