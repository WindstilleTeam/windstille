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

#include "display/display.hpp"
#include "font/fonts.hpp"
#include "input/controller.hpp"
#include "button.hpp"

namespace GUI {

Button::Button(const lisp::Lisp* lisp, Component* parent)
  : Component(parent)
{
  assert(0);
}

Button::Button(const std::string& label_, Component* parent)
  : Component(Rectf(), parent),
    label(label_)
{
}

Button::~Button()
{
}

void
Button::draw()
{
  Display::fill_rect(rect, Color(0.0f, 0.0f, 0.0f, 0.5f));
  Display::draw_rect(rect, Color(1.0f, 1.0f, 1.0f, 0.5f));
  Fonts::ttfdialog->draw_center(rect.left + rect.get_width()/2, rect.top + rect.get_height()/2,
                                label,
                                is_active()
                                ? Color(1.0f, 1.0f, 1.0f, 1.0f) 
                                : Color(1.0f, 1.0f, 1.0f, 0.5f));
}

void
Button::update(float , const Controller& controller)
{
  set_active(false);

  for(InputEventLst::const_iterator i = controller.get_events().begin(); i != controller.get_events().end(); ++i) 
    {
      if (i->type == BUTTON_EVENT && i->button.down)
        {
          if (i->button.name == OK_BUTTON)
            {
            }
          else if (i->button.name == CANCEL_BUTTON)
            {            
              set_active(false);
            }
        }
    }  
}

} // namespace GUI

/* EOF */
