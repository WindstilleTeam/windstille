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

#ifndef HEADER_WINDSTILLE_OBJECTS_DECAL_HPP
#define HEADER_WINDSTILLE_OBJECTS_DECAL_HPP

#include <memory>

#include <wstdisplay/fwd.hpp>
#include <wstdisplay/surface.hpp>

#include "engine/game_object.hpp"

class Decal : public GameObject
{
private:
  std::shared_ptr<wstdisplay::SurfaceDrawable> drawable;
  glm::vec2 pos;

public:
  Decal(ReaderMapping const& reader);
  ~Decal() override;

  void draw (wstdisplay::SceneContext& context) override;
  void update (float delta) override;

  void set_parent(GameObject* parent);

private:
  Decal(const Decal&);
  Decal& operator=(const Decal&);
};

#endif

/* EOF */
