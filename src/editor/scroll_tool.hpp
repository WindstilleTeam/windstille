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

#ifndef HEADER_WINDSTILLE_EDITOR_SCROLL_TOOL_HPP
#define HEADER_WINDSTILLE_EDITOR_SCROLL_TOOL_HPP

#include <wstdisplay/graphic_context_state.hpp>
#include "editor/tool.hpp"

namespace windstille {


/** */
class ScrollTool : public Tool
{
private:
  wstdisplay::GraphicContextState orig_state;
  glm::vec2 orig_click;
  enum { SCROLLING, NO_MODE } mode;

public:
  ScrollTool();

  void mouse_down(GdkEventButton* event, WindstilleWidget& wst) override;
  void mouse_move(GdkEventMotion* event, WindstilleWidget& wst) override;
  void mouse_up(GdkEventButton* event, WindstilleWidget& wst) override;

private:
  ScrollTool(ScrollTool const&);
  ScrollTool& operator=(ScrollTool const&);
};


} // namespace windstille

#endif

/* EOF */
