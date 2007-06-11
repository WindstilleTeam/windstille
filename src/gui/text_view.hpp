/*  $Id$
**   __      __ __             ___        __   __ __   __
**  /  \    /  \__| ____    __| _/_______/  |_|__|  | |  |   ____
**  \   \/\/   /  |/    \  / __ |/  ___/\   __\  |  | |  | _/ __ \
**   \        /|  |   |  \/ /_/ |\___ \  |  | |  |  |_|  |_\  ___/
**    \__/\  / |__|___|  /\____ /____  > |__| |__|____/____/\___  >
**         \/          \/      \/    \/                         \/
**  Copyright (C) 2005 Ingo Ruhnke <grumbel@gmx.de>
**
**  This program is free software; you can redistribute it and/or
**  modify it under the terms of the GNU General Public License
**  as published by the Free Software Foundation; either version 2
**  of the License, or (at your option) any later version.
**
**  This program is distributed in the hope that it will be useful,
**  but WITHOUT ANY WARRANTY; without even the implied warranty of
**  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
**  GNU General Public License for more details.
** 
**  You should have received a copy of the GNU General Public License
**  along with this program; if not, write to the Free Software
**  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA
**  02111-1307, USA.
*/

#ifndef HEADER_WINDSTILLE_GUI_TEXT_VIEW_HPP
#define HEADER_WINDSTILLE_GUI_TEXT_VIEW_HPP

#include "text_area.hpp"
#include "component.hpp"

namespace GUI {

/** */
class TextView : public Component
{
private:
  TextArea text_area;

public:
  TextView(const Rectf& rect, Component* component);
  ~TextView();
  
  void draw();
  void update(float delta, const Controller& controller);

  void set_screen_rect(const Rectf& rect);
  void set_text(const std::string& text);
  void set_font(TTFFont* font);

private:
  TextView (const TextView&);
  TextView& operator= (const TextView&);
};

} // namespace GUI

#endif

/* EOF */
