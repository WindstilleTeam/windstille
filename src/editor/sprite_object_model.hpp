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

#ifndef HEADER_WINDSTILLE_EDITOR_SPRITE_OBJECT_MODEL_HPP
#define HEADER_WINDSTILLE_EDITOR_SPRITE_OBJECT_MODEL_HPP

#include "sprite2d/sprite.hpp"
#include "editor/object_model.hpp"

namespace windstille {


class SpriteObjectModel : public ObjectModel
{
private:
  std::string path;
  Sprite sprite;

public:
  SpriteObjectModel(std::string const& name_, glm::vec2 const& rel_pos_,
                    std::string const& path_);
  ~SpriteObjectModel() override;

  void draw(wstdisplay::SceneContext& sc) override;
  void update(float delta) override;
  geom::frect get_bounding_box() const override;
  ObjectModelHandle clone() const override;
  void write(FileWriter& writer) const override;

  void add_to_scenegraph(wstdisplay::DrawableGroup& sg) override {}
};


} // namespace windstille

#endif

/* EOF */
