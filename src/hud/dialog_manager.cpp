/*
**  Windstille - A Sci-Fi Action-Adventure Game
**  Copyright (C) 2000,2005 Ingo Ruhnke <grumbel@gmail.com>
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

#include <glm/glm.hpp>
#include <glm/ext.hpp>

#include <geom/geom.hpp>
#include <wstinput/controller.hpp>

#include <wstdisplay/graphics_context.hpp>
#include <wstdisplay/opengl_window.hpp>
#include <wstdisplay/font/text_area.hpp>
#include <wstgui/style.hpp>

#include "app/app.hpp"
#include "app/controller_def.hpp"
#include "engine/script_manager.hpp"
#include "font/fonts.hpp"
#include "screen/game_session.hpp"
#include "util/pathname.hpp"

DialogManager::DialogManager() :
  portrait(),
  text(),
  delay(),
  text_area(),
  alignment(),
  caption(),
  outer_border_y()
{
}

DialogManager::~DialogManager()
{
}

void
DialogManager::add_dialog(int alignment_, std::string const& portrait_, std::string const& text_)
{
  delay     = 0.0;
  alignment = alignment_;
  portrait  = g_app.sprite().create(Pathname(portrait_));
  portrait.set_scale(0.65f);
  text      = text_;
  caption = false;

  create_text();
}

void
DialogManager::add_caption(int alignment_, std::string const& text_)
{
  delay     = 0.0;
  alignment = alignment_;
  text      = text_;
  caption = true;

  create_text();
}

void
DialogManager::draw(wstdisplay::GraphicsContext& gc)
{
  int dialog_height = std::max(portrait_height + portrait_border_y*2,
                               int(text_area->get_rect().height()
                                   + text_border_y * 2.0f));

  glm::vec2 pos(0.0f, 0.0f);
  if(alignment & LEFT) {
    pos.x = outer_border_x;
  } else if(alignment & RIGHT) {
    pos.x = static_cast<float>(gc.size().width() - dialog_width - outer_border_x);
  } else {
    pos.x = static_cast<float>((gc.size().width() - dialog_width) / 2);
  }

  if(alignment & TOP) {
    pos.y = static_cast<float>(outer_border_y);
  } else if(alignment & BOTTOM) {
    pos.y = static_cast<float>(gc.size().height() - dialog_height - outer_border_y);
  } else {
    pos.y = static_cast<float>((gc.size().height() - dialog_height) / 2);
  }

  if (!caption) {
    geom::frect rect(pos.x,
               pos.y,
               pos.x + dialog_width,
               pos.y + 200.0f);

    gc.fill_rounded_rect(rect, 16.0f,
                               surf::Color(0, 0, 0.3f, 0.5f));
    gc.draw_rounded_rect(rect, 16.0f,
                               surf::Color(0.6f, 1.0f, 1.0f, 0.8f));

    portrait.draw(gc, glm::vec2(pos.x + portrait_border_x,
                                pos.y + portrait_border_y));
  }

  text_area->draw(gc);

  if (text_area->is_progress_complete())
  {
    glm::vec2 const& pos_ = text_area->get_cursor_pos();
    geom::frect cursor(pos_.x + 8, pos_.y + 8, pos_.x + 24, pos_.y + 24);
    gc.fill_rect(cursor, surf::Color(1.0, 1.0, 1.0,
                                     fabsf(sinf(static_cast<float>(SDL_GetTicks()) / 1000.0f * glm::pi<float>() * 3.0f))));
  }
}

void
DialogManager::update(float delta, wstinput::Controller const& controller)
{
  text_area->update(delta);

  delay += delta;

  auto const& events = controller.get_events();

  for (auto i = events.begin(); i != events.end(); ++i)
  {
    if ((*i).type == wstinput::BUTTON_EVENT)
    {
      if ((*i).button.name == OK_BUTTON && (*i).button.down)
      {
        if (text_area->is_progress_complete())
        {
          GameSession::current()->set_control_state(GameSession::GAME);
          ScriptManager::current()->fire_wakeup_event(ScriptManager::DIALOG_CLOSED);
        }
        else if (delay > 0.2f)
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

  glm::ivec2 pos(0,0);
  if(alignment & LEFT) {
    pos.x = outer_border_x;
  } else if(alignment & RIGHT) {
    pos.x = g_app.window().get_gc().size().width() - dialog_width - outer_border_x;
  } else {
    pos.x = (g_app.window().get_gc().size().width() - dialog_width) / 2;
  }

  int text_width
    = dialog_width - portrait_height - portrait_border_x*2 - text_border_x;

  geom::irect text_rect = geom::irect(geom::ipoint(pos.x + portrait_width + portrait_border_x*2, 0),
                        geom::isize(500, 200)); // FIXME: use real bounding box calc

  text_rect = geom::irect(text_rect.left(),
                   text_rect.top(),
                   text_rect.right(),
                   text_rect.top() + text_rect.height());

  text_rect = geom::irect(text_rect.left(),
                   pos.y + text_border_y,
                   text_rect.right(),
                   text_rect.bottom());

  int dialog_height = std::max(portrait_height + portrait_border_y*2,
                               text_rect.height() + text_border_y*2);

  if(alignment & TOP) {
    pos.y = outer_border_y;
  } else if(alignment & BOTTOM) {
    pos.y = g_app.window().get_gc().size().height() - dialog_height - outer_border_y;
  } else {
    pos.y = (g_app.window().get_gc().size().height() - dialog_height) / 2;
  }

  text_rect = geom::irect(text_rect.left(),
                   text_rect.top(),
                   text_rect.right(),
                   text_rect.top() + text_rect.height());

  text_rect = geom::irect(text_rect.left(),
                   pos.y + text_border_y,
                   text_rect.right(),
                   text_rect.bottom());

  geom::isize dialog_size(dialog_width, dialog_height);

  text_area.reset(new wstdisplay::TextArea(g_app.style().get_font(),
                                           geom::frect(geom::irect(geom::ipoint(text_rect.left(), text_rect.top() + g_app.fonts().vera20->get_height()),
                                                                   geom::isize(text_width, 200))), true));
  text_area->set_font(g_app.fonts().vera20.get());
  text_area->set_text(text);
}

/* EOF */
