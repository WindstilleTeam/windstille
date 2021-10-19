/*
**  Windstille - A Sci-Fi Action-Adventure Game
**  Copyright (C) 2000,2005 Ingo Ruhnke <grumbel@gmail.com>
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

#ifndef HEADER_WINDSTILLE_OBJECTS_TRIGGER_HPP
#define HEADER_WINDSTILLE_OBJECTS_TRIGGER_HPP

#include <vector>

#include "engine/game_object.hpp"

/** */
class Trigger : public GameObject
{
private:
  geom::frect area;
  std::string callback;
  /// has the trigger been activated at least once
  bool triggered;
  /// trigger active in last frame
  bool last_trigger;
  bool one_time_trigger;

public:
  Trigger(ReaderMapping const& props);
  ~Trigger() override;

  void draw (wstdisplay::SceneContext& gc) override;
  void update (float delta) override;
};

#endif

/* EOF */
