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

#include "hud/dialog_manager.hpp"

#include <iostream>

#include "font/fonts.hpp"
#include "screen/game_session.hpp"
#include "input/controller.hpp"
#include "input/input_manager.hpp"
#include "engine/script_manager.hpp"
#include "display/text_area.hpp"
#include "display/display.hpp"
#include "util/pathname.hpp"
#include "app/config.hpp"

DialogManager* DialogManager::current_ = 0;

DialogManager::DialogManager()
{
  current_ = this;
}

DialogManager::~DialogManager()
{
}

void
DialogManager::add_dialog(int alignment_, const std::string& portrait_, const std::string& text_)
{ 
  delay     = 0.0;
  alignment = alignment_;
  portrait  = Sprite(Pathname(portrait_));
  portrait.set_scale(0.65f);
  text      = text_;
  caption = false;
  
  create_text();
}

void
DialogManager::add_caption(int alignment_, const std::string& text_)
{
  delay     = 0.0;
  alignment = alignment_;
  text      = text_;
  caption = true;
  
  create_text();
}

void
DialogManager::draw()
{
  int dialog_height = std::max(portrait_height + portrait_border_y*2,
                               int(text_area->get_rect().get_height()
                                   + text_border_y * 2.0f));

  Vector2f pos(0.0f, 0.0f);
  if(alignment & LEFT) {
    pos.x = outer_border_x;
  } else if(alignment & RIGHT) {
    pos.x = Display::get_width() - dialog_width - outer_border_x;
  } else {
    pos.x = (Display::get_width() - dialog_width) / 2;
  }

  if(alignment & TOP) {
    pos.y = outer_border_y;
  } else if(alignment & BOTTOM) {
    pos.y = Display::get_height() - dialog_height - outer_border_y;
  } else {
    pos.y = (Display::get_height() - dialog_height) / 2;
  }

  if (!caption) {
    Rectf rect(pos.x,
               pos.y,
               pos.x + dialog_width,
               pos.y + 200.0f);
    
    Display::fill_rounded_rect(rect, 16.0f,
                               Color(0, 0, 0.3f, 0.5f));
    Display::draw_rounded_rect(rect, 16.0f,
                               Color(0.6f, 1.0f, 1.0f, 0.8f));
  
    portrait.draw(Vector2f(pos.x + portrait_border_x,
                           pos.y + portrait_border_y));
  }
  
  text_area->draw();

  if (text_area->is_progress_complete())
    {
      const Vector2f& pos = text_area->get_cursor_pos();
      Rectf cursor(pos.x + 8, pos.y + 8, pos.x + 24, pos.y + 24);
      Display::fill_rect(cursor, Color(1.0, 1.0, 1.0, fabs(sin(SDL_GetTicks() / 1000.0f * M_PI * 3.0f))));
    }
}

void
DialogManager::update(float delta, const Controller& controller)
{
  text_area->update(delta);

  delay += delta;

  InputEventLst events = controller.get_events();
	
  for (InputEventLst::iterator i = events.begin(); i != events.end(); ++i)
    {
      if ((*i).type == BUTTON_EVENT)
        {
          if ((*i).button.name == OK_BUTTON && (*i).button.down == true)
            {
              if (text_area->is_progress_complete())
                {
                  GameSession::current()->set_control_state(GameSession::GAME);
                  ScriptManager::current()->fire_wakeup_event(ScriptManager::DIALOG_CLOSED);
                }
              else if (delay > 0.2)
                text_area->set_progress_complete();
            } 
        }
    }
}

void
DialogManager::create_text()
{
  if (caption)
    outer_border_y = 0;
  else
    outer_border_y = 20;
    
  Point pos(0,0);
  if(alignment & LEFT) {
    pos.x = outer_border_x;
  } else if(alignment & RIGHT) {
    pos.x = Display::get_width() - dialog_width - outer_border_x;
  } else {
    pos.x = (Display::get_width() - dialog_width) / 2;
  }
      
  int text_width
    = dialog_width - portrait_height - portrait_border_x*2 - text_border_x;
  Rect text_rect = Rect(Point(pos.x + portrait_width + portrait_border_x*2, 0),
                        Size(500, 200)); // FIXME: use real bounding box calc
  
  text_rect.bottom = text_rect.top + text_rect.get_height();
  text_rect.top    = pos.y + text_border_y;

  int dialog_height = std::max(portrait_height + portrait_border_y*2,
                               text_rect.get_height() + text_border_y*2);

  if(alignment & TOP) {
    pos.y = outer_border_y;
  } else if(alignment & BOTTOM) {
    pos.y = Display::get_height() - dialog_height - outer_border_y;
  } else {
    pos.y = (Display::get_height() - dialog_height) / 2;
  }

  text_rect.bottom = text_rect.top + text_rect.get_height();
  text_rect.top = pos.y + text_border_y;

  Size dialog_size(dialog_width, dialog_height);

  text_area.reset(new TextArea(Rect(Point(text_rect.left, text_rect.top + Fonts::current()->vera20->get_height()),
                                    Size(text_width, 200)), true));
  text_area->set_font(Fonts::current()->vera20.get());
  text_area->set_text(text);
}

/* EOF */
