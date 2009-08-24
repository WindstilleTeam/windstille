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

#include <stdint.h>

#include "math/rect.hpp"
#include "editor/object_model.hpp"
#include "editor/tool.hpp"

class SelectTool : public Tool
{
private:
  Vector2f click_pos;
  Rectf    rect;
  SelectionHandle selection;
  ControlPointHandle ctrl_point;
  uint32_t start_time;
  
  enum { 
    SELECT_MODE,
    OBJECT_DRAG_MODE,
    CONTROL_DRAG_MODE,
    NO_MODE
  } mode;

  Vector2f process_snap(WindstilleWidget& wst);

public:
  SelectTool();
  
  void mouse_down (GdkEventButton* event, WindstilleWidget& wst);
  void mouse_up(GdkEventButton* event, WindstilleWidget& wst);
  void mouse_move(GdkEventMotion* event, WindstilleWidget& wst);

  void mouse_right_down(GdkEventButton* event, WindstilleWidget& wst);

  void draw(SceneContext& sc);

private:
  SelectTool(const SelectTool&);
  SelectTool& operator=(const SelectTool&);
};

#endif

/* EOF */
