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

#include <wstdisplay/surface.hpp>
#include <wstgui/component.hpp>

namespace wstgui {

class Automap : public Component
{
private:
  wstdisplay::SurfacePtr surface;
  glm::vec2  pos;
  float   zoom;

public:
  Automap(Component* parent);
  ~Automap() override;

  void draw(wstdisplay::GraphicsContext& gc) override;
  void update(float delta, wstinput::Controller const& controller) override;

private:
  Automap (Automap const&);
  Automap& operator= (Automap const&);
};

} // namespace wstgui

#endif

/* EOF */
