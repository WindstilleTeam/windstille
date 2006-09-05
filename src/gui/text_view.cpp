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

#include "text_view.hpp"

namespace GUI {

TextView::TextView(const Rectf& rect, Component* component)
  : Component(rect, component),
    text_area(rect, false)
{
}

TextView::~TextView()
{
}
  
void
TextView::draw()
{
  text_area.draw();
}

void
TextView::update(float delta, const Controller& controller)
{
  text_area.update(delta);
  set_active(false);
}

void
TextView::set_screen_rect(const Rectf& rect)
{
  Component::set_screen_rect(rect);
  text_area.set_rect(rect);
}

void
TextView::set_text(const std::string& text)
{
  text_area.set_text(text);
}

} // namespace GUI

/* EOF */
