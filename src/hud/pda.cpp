/*
**  Windstille - A Sci-Fi Action-Adventure Game
**  Copyright (C) 2005 Ingo Ruhnke <grumbel@gmx.de>
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

#include "input/input_manager.hpp"
#include "display/display.hpp"
#include "display/text_area.hpp"
#include "font/fonts.hpp"
#include "pda.hpp"

DialogEntry::DialogEntry(const std::string& arg_character, const std::string& arg_text)
  : character(arg_character),
    text(arg_text)
{
}

ObjectiveEntry::ObjectiveEntry(const std::string& arg_name, const std::string& arg_text)
  : name(arg_name),
    text(arg_text),
    complete(false)
{
}

PDA::PDA()
  : state(PDA_OBJECTIVES)
{ 
  background = Sprite("images/pda/pda.sprite");
}

PDA::~PDA()
{
}

void
PDA::draw()
{
  if (text_area.get())
    {
      // Darken the background a bit
      Display::fill_rect(Rect(0, 0, Display::get_width(), Display::get_height()), Color(0.0f, 0.0f, 0.0f, 0.25f));

      Rectf rect = text_area->get_rect().grow(8.0f);

      background.draw(Vector2f(30, 30));
      
      rect.top += 56;
      Display::fill_rounded_rect(rect, 16.0f, Color(0.3f, 0.3f, 0.5f, 0.5f));
      Display::draw_rounded_rect(rect, 16.0f, Color(1.0f, 1.0f, 1.0f, 0.5f));

      text_area->draw();
    }
}

void
PDA::update(float delta, const Controller& controller)
{
  const InputEventLst& events = controller.get_events();

  for(InputEventLst::const_iterator i = events.begin(); i != events.end(); ++i) 
    {
      if (i->type == BUTTON_EVENT)
        {
          if (i->axis.name == MENU_LEFT_BUTTON && i->button.down) 
            {
              state = static_cast<pda_state>(state + 1);
              if (state > PDA_DIALOGS)
                state = PDA_OBJECTIVES;
            }
          else if (i->axis.name == MENU_RIGHT_BUTTON && i->button.down) 
            {
              state = static_cast<pda_state>(state - 1);
              if (state < PDA_OBJECTIVES)
                state = PDA_DIALOGS;
            }
        }
    }
  
  if (text_area.get())
    text_area->set_scroll_offset(text_area->get_scroll_offset() + 500.0f * controller.get_axis_state(Y2_AXIS) * delta);
    
  switch (state) 
    {
      case PDA_OBJECTIVES:
        show_objectives();
        break;

      case PDA_DIALOGS:
        show_dialogs();
        break;
    }
 
  if (new_text != old_text) 
    {
      // FIXME: Unneeded could just use set_text
      text_area.reset(new TextArea(Rectf(70, 83, 385, 520).grow(-12.0f), false));
      text_area->set_font(Fonts::ttffont);
      text_area->set_text(new_text);
    
      old_text = new_text;
    }
  
  text_area->update(delta);
}

void
PDA::add_dialog(const std::string& character, const std::string& text)
{
  dialogs.push_back(DialogEntry(character, text));
}

void
PDA::add_objective(const std::string& name, const std::string& text)
{
  objectives.push_back(ObjectiveEntry(name, text));
}

void
PDA::objective_complete(const std::string& name)
{
  for (std::vector<ObjectiveEntry>::iterator i = objectives.begin(); i != objectives.end(); ++i) 
    {
      if (i->name == name)
        i->complete = true;
      return;
    }
}

bool
PDA::is_objective_given(const std::string& name)
{
  for (std::vector<ObjectiveEntry>::iterator i = objectives.begin(); i != objectives.end(); ++i) 
    {
      if (i->name == name)
        return true;
    }
  
  return false;
}

bool
PDA::is_objective_complete(const std::string& name)
{
  for (std::vector<ObjectiveEntry>::iterator i = objectives.begin(); i != objectives.end(); ++i) 
    {
      if (i->name == name && i->complete)
        return true;
    }
  
  return false;
}

void
PDA::show_objectives()
{
  new_text = "<large>Personal Digital Assistant</large>\n";
  new_text += "<b>objectives</b> - dialogs\n\n";
  
  for (std::vector<ObjectiveEntry>::reverse_iterator i = objectives.rbegin(); i != objectives.rend(); ++i) 
    {
      new_text += i->name;
      new_text += ": ";

      if (i->complete)
        new_text += "complete\n";
      else
        new_text += "incomplete\n";

      new_text += i->text + "\n\n";
    }
}

void
PDA::show_dialogs()
{
  new_text = "<large>Personal Digital Assistant</large>\n";
  new_text += "objectives - <b>dialogs</b>\n\n";
  
  for (std::vector<DialogEntry>::reverse_iterator i = dialogs.rbegin(); i != dialogs.rend(); ++i) 
    {
      new_text += i->character;
      new_text += ": ";
      new_text += i->text + '\n';
    }
}

/* EOF */
