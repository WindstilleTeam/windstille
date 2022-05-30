/*
**  Windstille - A Sci-Fi Action-Adventure Game
**  Copyright (C) 2009 Ingo Ruhnke <grumbel@gmail.com>
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

#include "editor/sprite_object_model.hpp"

#include "app/app.hpp"
#include "util/pathname.hpp"
#include <wstdisplay/scene_context.hpp>

namespace windstille {

SpriteObjectModel::SpriteObjectModel(std::string const& name_, glm::vec2 const& rel_pos_,
                                     std::string const& path_) :
  ObjectModel(name_, rel_pos_),
  path(path_),
  sprite(g_app.sprite().create(Pathname(path_)))
{
}

SpriteObjectModel::~SpriteObjectModel()
{
}

void
SpriteObjectModel::draw(wstdisplay::SceneContext& sc)
{
  sprite.draw(sc.color(), get_world_pos());
}

void
SpriteObjectModel::update(float delta)
{
  sprite.update(delta);
}

geom::frect
SpriteObjectModel::get_bounding_box() const
{
  return geom::frect(get_world_pos(), geom::fsize(sprite.get_width(), sprite.get_height()));
}

ObjectModelHandle
SpriteObjectModel::clone() const
{
  return ObjectModelHandle(new SpriteObjectModel(*this));
}

void
SpriteObjectModel::write(FileWriter& writer) const
{
  writer.begin_object("sprite");
  ObjectModel::write_member(writer);
  writer.write("path", path);
  writer.end_object();
}


} // namespace windstille

/* EOF */
