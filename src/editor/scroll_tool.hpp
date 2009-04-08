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

#ifndef HEADER_WINDSTILLE_EDITOR_SCROLL_TOOL_HPP
#define HEADER_WINDSTILLE_EDITOR_SCROLL_TOOL_HPP

#include "display/graphic_context_state.hpp"
#include "tool.hpp"

/** */
class ScrollTool : public Tool
{
private:
  GraphicContextState orig_state;
  Vector2f orig_click;
  enum { SCROLLING, NO_MODE } mode;

public:
  ScrollTool();

  virtual void mouse_down(GdkEventButton* event, WindstilleWidget& wst);
  virtual void mouse_move(GdkEventMotion* event, WindstilleWidget& wst);
  virtual void mouse_up(GdkEventButton* event, WindstilleWidget& wst);

private:
  ScrollTool(const ScrollTool&);
  ScrollTool& operator=(const ScrollTool&);
};

#endif

/* EOF */
