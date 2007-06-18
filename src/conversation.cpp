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

#include "math.hpp"
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
  : pos(400, 300)
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
  
  Display::fill_circle(pos, 42.0f, Color(1.0f, 1.0f, 1.0f, 0.25f), 24);
  Display::draw_circle(pos, 42.0f, Color(1.0f, 1.0f, 1.0f, 0.25f), 24);

  float segment = 360.0f / choices.size();

  for(int i = 0; i < choices.size(); ++i)
    { // FIXME:
      Vector offset(0.0f, 1.0f);
      offset = offset.rotate(math::deg2rad(segment*i) - M_PI/2);

      float start = -segment/2 - 90.0f + segment*i;
      float end   = -segment/2 - 90.0f + segment*(i+1);
      
      // FIXME: Doesn't handle multi line text
      Sizef size(Fonts::vera20->get_width(choices[i]) + 10,
                 Fonts::vera20->get_height() + 10);
      Rectf rect(pos + 128.0f * offset - Vector(size.width/2, size.height - 5), size);

      if (i == selection)
        {
          Display::fill_arc(pos + (5.0f * offset), 32.0f, 
                            start, end,
                            Color(1.0f, 1.0f, 1.0f, 1.0f), 24);
          Display::fill_rounded_rect(rect, 5.0f, Color(0.5f, 0.5f, 0.5f, 1.0f));
          Fonts::vera20->draw_center(pos.x + 128.0f * offset.x, pos.y + 128.0f * offset.y, choices[i], Color(1.0f, 1.0f, 1.0f));
        }
      else
        {
          Display::fill_rounded_rect(rect, 5.0f, Color(0.5f, 0.5f, 0.5f, 0.25f));
          Fonts::vera20->draw_center(pos.x + 128.0f * offset.x, pos.y + 128.0f * offset.y, choices[i], Color(0.5f, 0.5f, 0.5f));
        }

      Display::draw_arc(pos + 5.0f * offset, 32.0f,
                        start, end,
                        Color(1.0f, 1.0f, 1.0f, 1.0f), 24);

    }

  Display::draw_line(pos, pos + direction*32.0f, Color(1.0f, 1.0f, 1.0f));

  /*
    cosf(angle) * radius + pos.x;
    sinf(angle) * radius + pos.y;

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
  */
}

void
Conversation::update(float delta, const Controller& controller)
{
  if (!active)
    return;

  if (fabs(controller.get_axis_state(X_AXIS)) > 0.3f ||
      fabs(controller.get_axis_state(Y_AXIS)) > 0.3f)
    {
      direction = Vector(controller.get_axis_state(X_AXIS),
                         controller.get_axis_state(Y_AXIS));

      float segment = 360.0f / choices.size();
      float angle = math::rad2deg(math::normalize_angle(atan2f(direction.y, direction.x) + M_PI/2 + math::deg2rad(segment/2.0f)));

      selection = int(angle / segment);
      selection = math::mid(0, selection, int(choices.size()));
    }
  else
    {
    }

  /*
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
  */
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
