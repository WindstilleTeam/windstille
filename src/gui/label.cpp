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

#include "font/fonts.hpp"
#include "label.hpp"

namespace gui {

Label::Label(const std::string& label_)
  : Component(Rectf(), parent),
    label(label_)
{
}

Label::~Label()
{
}

void
Label::draw()
{
  //Display::fill_rect(rect, Color(0.0f, 0.0f, 0.0f, 0.5f));
  //Display::draw_rect(rect, Color(1.0f, 1.0f, 1.0f, 0.5f));
  Fonts::vera12->draw(int(rect.left + 5/*+ rect.get_width()/2*/), int(rect.top + rect.get_height()/2 + 3),
                      label,
                      Color(1.0f, 1.0f, 1.0f, 1.0f));
}

void
Label::update(float delta, const Controller& controller)
{
  set_active(false);
}

} // namespace gui

/* EOF */
