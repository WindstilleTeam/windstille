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

#ifndef HEADER_WINDSTILLE_OBJECTS_NIGHTVISION_HPP
#define HEADER_WINDSTILLE_OBJECTS_NIGHTVISION_HPP

#include <wstdisplay/texture.hpp>
#include "engine/game_object.hpp"
#include "sprite2d/sprite.hpp"

namespace windstille {

// FIXME: shouldn't really be a game object, but makes testing easier
class Nightvision : public GameObject
{
private:
  Sprite  nightvision;
  wstdisplay::TexturePtr noise;

public:
  Nightvision(ReaderMapping const& props);
  ~Nightvision() override;

  void draw(wstdisplay::SceneContext& sc) override;
  void update(float delta) override;

private:
  Nightvision (Nightvision const&);
  Nightvision& operator= (Nightvision const&);
};

} // namespace windstille

#endif

/* EOF */
