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

#ifndef HEADER_WINDSTILLE_OBJECTS_LAYER_HPP
#define HEADER_WINDSTILLE_OBJECTS_LAYER_HPP

#include "engine/game_object.hpp"

namespace windstille {

class Layer : public GameObject
{
private:
  typedef std::vector<std::shared_ptr<GameObject> > Objects;

  Objects objects;

  /** container for newly created GameObjects (they'll be added once per frame
   * in the update function */
  Objects new_objects;

private:
  void commit_adds();
  void commit_removes();

public:
  Layer(ReaderMapping const& reader);
  ~Layer() override;

  void draw (wstdisplay::SceneContext& context) override;
  void update (float delta) override;


private:
  Layer(Layer const&);
  Layer& operator=(Layer const&);
};

} // namespace windstille

#endif

/* EOF */
