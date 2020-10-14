/*
**  Windstille - A Sci-Fi Action-Adventure Game
**  Copyright (C) 2005 Ingo Ruhnke <grumbel@gmail.com>
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

#include "gui/menu_component.hpp"

#include <wstinput/controller.hpp>

#include "app/app.hpp"
#include "app/controller_def.hpp"
#include "app/sound_manager.hpp"
#include "display/display.hpp"
#include "gui/menu_item.hpp"
#include "gui/tab_component.hpp"
#include "util/pathname.hpp"

namespace gui {

MenuComponent::MenuComponent(const geom::frect& rect_, bool allow_cancel_, Component* parent_)
  : Component(rect_, parent_),
    items(),
    current_item(0),
    font(g_app.fonts().vera20.get()),
    allow_cancel(allow_cancel_),
    scroll_mode(false),
    scroll_offset(0),
    num_displayable_items(-1)
{
}

MenuComponent::~MenuComponent()
{
  for(Items::iterator i = items.begin(); i != items.end(); ++i)
    delete *i;
  items.clear();
}

void
MenuComponent::add_item(MenuItem* item)
{
  items.push_back(item);

  if (calc_height() >= rect.height())
  {
    scroll_mode   = true;
    scroll_offset = 0;
    num_displayable_items = static_cast<int>(rect.height() / item_height());
  }
}

void
MenuComponent::draw()
{
  float step = item_height();

  if (scroll_mode)
  { // we can only display a subset of items and have to scroll
    for(int i = 0; i < num_displayable_items; ++i)
    {
      items[i+scroll_offset]->draw(geom::frect(rect.left(), rect.top() + static_cast<float>(i) * step + 2.0f,
                                         rect.right() - 32.0f, rect.top() + static_cast<float>(i+1) * step - 2.0f),
                                   is_active() && (i + scroll_offset == current_item));
    }

    // draw scrollbar
    float scrollbar_height = (rect.height() - 4.0f) * static_cast<float>(num_displayable_items) / static_cast<float>(items.size());
    float scrollbar_incr   = (rect.height() - 4.0f) * static_cast<float>(scroll_offset) / static_cast<float>(items.size());

    Display::fill_rounded_rect(geom::frect(rect.right() - 24, rect.top() + 2.0f + scrollbar_incr,
                                     rect.right() - 2,  rect.top() + 2.0f + scrollbar_incr + scrollbar_height),
                               5.0f,
                               Color(0.5f, 0.5f, 0.5f, 0.75f));

    Display::draw_rounded_rect(geom::frect(rect.right() - 24, rect.top() + 2.0f,
                                     rect.right() - 2,  rect.bottom() - 2.0f),
                               5.0f,
                               Color(1.0f, 1.0f, 1.0f, 1.0f));
  }
  else
  { // all items fit on the screen
    for(Items::size_type i = 0; i < items.size(); ++i)
    {
      items[i]->draw(geom::frect(rect.left(), rect.top() + static_cast<float>(i) * step + 2.0f,
                           rect.right(), rect.top() + static_cast<float>(i+1) * step - 2.0f),
                     is_active() && (int(i) == current_item));
    }
  }
}

void
MenuComponent::update(float delta, const Controller& controller)
{
  for(Items::size_type i = 0; i < items.size(); ++i)
  {
    items[i]->update(delta);
  }

  for(auto i = controller.get_events().begin();
      i != controller.get_events().end();
      ++i)
  {
    if (i->type == wstinput::BUTTON_EVENT && i->button.down)
    {
      if (i->button.name == OK_BUTTON || i->button.name == ENTER_BUTTON)
      {
        items[current_item]->click();
      }
      else if (i->button.name == CANCEL_BUTTON ||
               i->button.name == ESCAPE_BUTTON ||
               i->button.name == PAUSE_BUTTON)
      {
        if (allow_cancel) // FIXME: Could use a signal instead
        {
          g_app.sound().play(Pathname("sounds/menu_click.wav", Pathname::kDataPath));
          set_active(false);
        }
      }
      else if (i->button.name == MENU_LEFT_BUTTON)
      {
        items[current_item]->incr();
      }
      else if (i->button.name == MENU_RIGHT_BUTTON)
      {
        items[current_item]->decr();
      }
      else if (i->button.name == MENU_UP_BUTTON)
      {
        g_app.sound().play(Pathname("sounds/menu_change.wav", Pathname::kDataPath));

        current_item = current_item - 1;
        if (current_item < 0)
        {
          if (dynamic_cast<TabComponent*>(parent))
          {
            current_item = 0;
            set_active(false);
          }
          else
          {
            current_item = static_cast<int>(items.size())-1;
          }
        }

        adjust_scroll_offset();
      }
      else if (i->button.name == MENU_DOWN_BUTTON)
      {
        g_app.sound().play(Pathname("sounds/menu_change.wav", Pathname::kDataPath));

        if (dynamic_cast<TabComponent*>(parent))
        {
          current_item = std::clamp(current_item + 1, 0, static_cast<int>(items.size()-1));
        }
        else
        {
          current_item += 1;
          if (current_item >= static_cast<int>(items.size()))
          {
            current_item = 0;
          }

        }

        adjust_scroll_offset();
      }
    }
  }
}

void
MenuComponent::set_font(TTFFont* font_)
{
  font = font_;
}

TTFFont*
MenuComponent::get_font()
{
  return font;
}

float
MenuComponent::get_prefered_width() const
{
  /*
    float width = 0;
    for(Items::iterator i = items.begin(); i != items.end(); ++i)
    {
    width = std::max(get_width())
    }  */
  return rect.width();
}

float
MenuComponent::get_prefered_height() const
{
  return item_height() * static_cast<float>(std::min(10, int(items.size()))) + 12.0f;
}

float
MenuComponent::calc_height()
{
  return static_cast<float>(items.size()) * item_height();
}

float
MenuComponent::item_height() const
{
  float spacing = 10.0f;
  return static_cast<float>(font->get_height()) + spacing * 2.0f;
}

void
MenuComponent::adjust_scroll_offset()
{
  if (scroll_mode)
  {
    if (current_item - scroll_offset >= num_displayable_items)
    { // scrolling down
      scroll_offset = current_item - (num_displayable_items-1);
    }
    else if (current_item < scroll_offset)
    { // scrolling up
      scroll_offset = current_item;
    }
  }
}

void
MenuComponent::set_screen_rect(const geom::frect& rect_)
{
  num_displayable_items = static_cast<int>(rect_.height() / item_height());

  if (num_displayable_items < int(items.size()))
  {
    scroll_mode   = true;
    scroll_offset = 0;
  }
  else
  {
    scroll_mode   = false;
    scroll_offset = 0;
  }


  Component::set_screen_rect(rect_);
}

} // namespace gui

/* EOF */
