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

#ifndef HEADER_WINDSTILLE_EDITOR_SELECT_TOOL_HPP
#define HEADER_WINDSTILLE_EDITOR_SELECT_TOOL_HPP

#include <geom/geom.hpp>

#include "editor/object_model.hpp"
#include "editor/tool.hpp"

namespace windstille {


class SelectTool : public Tool
{
private:
  EditorWindow& m_editor;
  glm::vec2 click_pos;
  geom::frect    rect;
  SelectionHandle selection;
  ControlPointHandle ctrl_point;
  guint32 start_time;

  enum {
    SELECT_MODE,
    OBJECT_DRAG_MODE,
    CONTROL_DRAG_MODE,
    NO_MODE
  } mode;

  glm::vec2 process_snap(WindstilleWidget& wst);
  glm::vec2 process_grid_snap(WindstilleWidget& wst);

public:
  SelectTool(EditorWindow& editor);

  void mouse_down (GdkEventButton* event, WindstilleWidget& wst) override;
  void mouse_up(GdkEventButton* event, WindstilleWidget& wst) override;
  void mouse_move(GdkEventMotion* event, WindstilleWidget& wst) override;

  void mouse_right_down(GdkEventButton* event, WindstilleWidget& wst) override;

  void draw(wstdisplay::SceneContext& sc) override;

private:
  SelectTool(SelectTool const&);
  SelectTool& operator=(SelectTool const&);
};


} // namespace windstille

#endif

/* EOF */
