/*
**  Windstille - A Sci-Fi Action-Adventure Game
**  Copyright (C) 2009 Ingo Ruhnke <grumbel@gmx.de>
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

#ifndef HEADER_WINDSTILLE_EDITOR_SELECT_TOOL_HPP
#define HEADER_WINDSTILLE_EDITOR_SELECT_TOOL_HPP

#include "math/rect.hpp"
#include "tool.hpp"

class SelectTool : public Tool
{
private:
  Vector2f click_pos;
  Rectf    rect;
  bool     rect_valid;

public:
  SelectTool();
  
  bool mouse_down (GdkEventButton* event, WindstilleWidget* wst);
  bool mouse_move(GdkEventMotion* event, WindstilleWidget* wst);
  bool mouse_up(GdkEventButton* event, WindstilleWidget* wst);
  void draw(SceneContext& sc);

private:
  SelectTool(const SelectTool&);
  SelectTool& operator=(const SelectTool&);
};

#endif

/* EOF */
