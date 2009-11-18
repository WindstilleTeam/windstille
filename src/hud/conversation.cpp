/*
**  Windstille - A Sci-Fi Action-Adventure Game
**  Copyright (C) 2000,2005 Ingo Ruhnke <grumbel@gmx.de>
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

#include "input/controller.hpp"
#include "font/fonts.hpp"
#include "engine/script_manager.hpp"
#include "display/display.hpp"
#include "hud/conversation.hpp"
#include "hud/pda.hpp"
#include "screen/game_session.hpp"

Conversation::Conversation()
  : pos(400, 300),
    time(0)
{
  active = false;
  selection = 0;
}

void
Conversation::add(const std::string& text)
{
  if (!active)
    {
      choices.push_back(Choice(text, ""));
      if (selection >= int(choices.size()))
        selection = 0;
    }
}

void
Conversation::add(const std::string& topic, const std::string& text)
{
  if (!active)
    {
      choices.push_back(Choice(topic, text));
      if (selection >= int(choices.size()))
        selection = 0;
    }
}

void
Conversation::draw()
{
  if (!active)
    return;
  
  Display::fill_circle(pos, 42.0f, Color(0.5f, 0.5f, 0.5f, 0.75f), 24);

  float segment = 360.0f / static_cast<float>(choices.size());

  for(int i = 0; i < static_cast<int>(choices.size()); ++i)
    { // FIXME:
      Vector2f offset(0.0f, 1.0f);
      offset = offset.rotate(math::deg2rad(segment * static_cast<float>(i)) - math::pi/2.0f);

      float start = -segment / 2.0f - 90.0f + segment * static_cast<float>(i);
      float end   = -segment / 2.0f - 90.0f + segment * static_cast<float>(i+1);
      
      float distance = 160.0f;

      Vector2f textpos = pos + Vector2f(0, 16.0f);
      // FIXME: Doesn't handle multi line text
      Sizef size(static_cast<float>(Fonts::current()->vera20->get_width(choices[i].topic)) + 40.0f,
                 static_cast<float>(Fonts::current()->vera20->get_height()) + 25.0f);
      Rectf  rect(textpos + distance * offset - Vector2f(size.width/2, size.height - 15), size);

      if (i == selection)
        {
          rect = rect.grow(grow);
          
          Display::fill_arc(pos, 42.0f, start, end, Color(1.0f, 1.0f, 1.0f, 0.5f), 24);
          Display::fill_rounded_rect(rect, 5.0f, Color(0.5f, 0.5f, 0.5f, 0.75f));

          Fonts::current()->vera20->draw_center(Vector2f(textpos.x + distance * offset.x,
                                                         textpos.y + distance * offset.y), 
                                                choices[i].topic, Color(1.0f, 1.0f, 0.0f));
          
          Fonts::current()->vera20->draw_center(Vector2f(400.0f, static_cast<float>(Display::get_height()) - 32.0f),
                                                choices[i].text, Color(1.0f, 1.0f, 1.0f));
          Display::draw_rounded_rect(rect, 5.0f, Color(1.0f, 1.0f, 0.0f));
        }
      else
        {
          Display::fill_rounded_rect(rect, 5.0f, Color(0.25f, 0.25f, 0.25f, 0.75f));
          Fonts::current()->vera20->draw_center(Vector2f(textpos.x + distance * offset.x,
                                            textpos.y + distance * offset.y),
                                     choices[i].topic, Color(0.8f, 0.8f, 0.8f));
          Display::draw_rounded_rect(rect, 5.0f, Color(1.0f, 1.0f, 1.0f));
        }

      //Display::draw_arc(pos + 5.0f * offset, 32.0f,
      //                  start, end,
      //                  Color(1.0f, 1.0f, 1.0f, 1.0f), 24);

    }
  Display::draw_circle(pos, 42.0f, Color(1.0f, 1.0f, 1.0f, 0.5f), 24);

  Display::fill_circle(pos + direction * 34.0f, 8.0f, Color(1.0f, 1.0f, 1.0f));
  //Display::draw_line(pos, pos + direction*32.0f, Color(0.0f, 0.0f, 0.0f));
}

void
Conversation::update(float delta, const Controller& controller)
{
  time += delta;

  if (!active)
    return;

  grow = fabsf(sinf(time * 3.0f)) * 4.0f;

  direction = Vector2f(controller.get_axis_state(X_AXIS),
                       controller.get_axis_state(Y_AXIS));

  if (fabs(controller.get_axis_state(X_AXIS)) > 0.3f ||
      fabs(controller.get_axis_state(Y_AXIS)) > 0.3f)
    {
      float segment = 360.0f / static_cast<float>(choices.size());
      float angle = math::rad2deg(math::normalize_angle(atan2f(direction.y, direction.x) + math::pi/2.0f + math::deg2rad(segment/2.0f)));

      int new_selection = int(angle / segment);
      new_selection = math::mid(0, new_selection, int(choices.size()));

      if (new_selection != selection) {
        selection = new_selection;
        // FIXME: Might be a good idea to do the woople-size per button, not globaly
        grow = 0.0f;
        time = 0;
      }
    }
  else
    {
      selection = -1;
    }
  
  if (controller.button_was_pressed(OK_BUTTON) && selection != -1)
    {
      active = false;
      GameSession::current()->get_pda().add_dialog("Jane",
                                                   choices[selection].topic + " - " +
                                                   choices[selection].text);
      choices.clear();
      GameSession::current()->set_control_state(GameSession::GAME);
      ScriptManager::current()->fire_wakeup_event(ScriptManager::CONVERSATION_CLOSED);
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
