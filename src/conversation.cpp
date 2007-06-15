/*  $Id$
**   __      __ __             ___        __   __ __   __
**  /  \    /  \__| ____    __| _/_______/  |_|__|  | |  |   ____
**  \   \/\/   /  |/    \  / __ |/  ___/\   __\  |  | |  | _/ __ \
**   \        /|  |   |  \/ /_/ |\___ \  |  | |  |  |_|  |_\  ___/
**    \__/\  / |__|___|  /\____ /____  > |__| |__|____/____/\___  >
**         \/          \/      \/    \/                         \/
**  Copyright (C) 2000,2005 Ingo Ruhnke <grumbel@gmx.de>
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

#include "input/controller.hpp"
#include "input/input_manager.hpp"
#include "font/fonts.hpp"
#include "math/rect.hpp"
#include "script_manager.hpp"
#include "display/display.hpp"
#include "conversation.hpp"
#include "pda.hpp"
#include "game_session.hpp"

Conversation* Conversation::current_ = 0;

Conversation::Conversation()
{
  current_ = this;
  active = false;
  selection = 0;
}

void
Conversation::add(const std::string& text)
{
  if (!active)
    {
      choices.push_back(text);
      if (selection >= int(choices.size()))
        selection = 0;
    }
}

void
Conversation::draw()
{
  if (!active)
    return;
    
  int x = 100;
  int y = 200;

  Rect rect(Point(x - 20, y - 20 + Fonts::ttffont->get_height()/2 - 5),
            Size(300 + 20, // FIXME:
                 (Fonts::ttffont->get_height() + 10) * choices.size() + 20));
  
  Display::fill_rect(rect, Color(0,0,0,0.5f));
  Display::draw_rect(rect, Color(1.0f, 1.0f, 1.0f, 0.3f));

  for(int i = 0; i < int(choices.size()); ++i)
    {
      if (i == selection)
        Fonts::vera20->draw(x, y, choices[i]);
      else
        Fonts::vera20->draw(x, y, choices[i], Color(0.5f, 0.5f, 0.5f));
  
      y += Fonts::vera20->get_height() + 10;
    }
}

void
Conversation::update(float delta, const Controller& controller)
{
  (void) delta;
  if (!active)
    return;

  if (controller.button_was_pressed(MENU_UP_BUTTON))
    {
      selection -= 1;
      if (selection < 0)
        selection = choices.size() - 1;
    }
  else if (controller.button_was_pressed(MENU_DOWN_BUTTON))
    {
      selection += 1;
      if (selection >= int(choices.size()))
        selection = 0;
    }
  
  if (controller.button_was_pressed(OK_BUTTON))
    {
      active = false;
      GameSession::current()->get_pda().add_dialog("Jane", choices[selection]);
      choices.clear();
      GameSession::current()->set_control_state(GameSession::GAME);
      script_manager->fire_wakeup_event(ScriptManager::CONVERSATION_CLOSED);
    }
}

int
Conversation::get_selection() const
{
  return selection;
}

void
Conversation::show()
{
  active = true;
}

/* EOF */
