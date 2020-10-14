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

#ifndef HEADER_WINDSTILLE_GUI_AUTOMAP_HPP
#define HEADER_WINDSTILLE_GUI_AUTOMAP_HPP

#include "display/surface.hpp"
#include "gui/component.hpp"

namespace gui {

/** */
class Automap : public Component
{
private:
  SurfacePtr surface;
  glm::vec2  pos;
  float   zoom;

public:
  Automap(Component* parent);
  Automap(const geom::frect& rect, Component* parent);
  ~Automap() override;

  void draw() override;
  void update(float delta, const Controller& controller) override;

private:
  Automap (const Automap&);
  Automap& operator= (const Automap&);
};

} // namespace gui

#endif

/* EOF */
