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

#ifndef HEADER_WINDSTILLE_GUI_LABEL_HPP
#define HEADER_WINDSTILLE_GUI_LABEL_HPP

#include "component.hpp"

namespace wstgui {

/** */
class Label : public Component
{
private:
  std::string label;

public:
  Label(const std::string& label, Component* parent);
  ~Label() override;

  void draw(wstdisplay::GraphicsContext& gc) override;
  void update(float delta, const Controller& controller) override;

private:
  Label (const Label&);
  Label& operator= (const Label&);
};

} // namespace wstgui

#endif

/* EOF */
