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

#include <iostream>
#include "sound/sound_manager.hpp"
#include "input/controller.hpp"
#include "menu_component.hpp"
#include "gui/tab_component.hpp"
#include "display/display.hpp"
#include "menu_item.hpp"
#include "math.hpp"

namespace gui {

MenuComponent::MenuComponent(const Rectf& rect, bool allow_cancel_, Component* parent)
  : Component(rect, parent),
    current_item(0),
    font(Fonts::vera16),
    allow_cancel(allow_cancel_),
    scroll_mode(false),
    scroll_offset(0),
    num_displayable_items(-1)
{
}

MenuComponent::~MenuComponent()
{
  for(Items::iterator i = items.begin(); i != items.end(); ++i)
    {
      delete *i;
    }
}

void
MenuComponent::add_item(MenuItem* item)
{
  items.push_back(item);
  
  if (calc_height() >= rect.get_height())
    {
      scroll_mode   = true;
      scroll_offset = 0;
      num_displayable_items = static_cast<int>(rect.get_height() / item_height());
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
          items[i+scroll_offset]->draw(Rectf(rect.left, rect.top + i * step + 2.0f, 
                                             rect.right - 32, rect.top + (i+1) * step - 2.0f), 
                                       is_active() && (int(i+scroll_offset) == current_item));
        }
      
      // draw scrollbar
      float scrollbar_height = (rect.get_height()-4.0f) * num_displayable_items / items.size();
      float scrollbar_incr   = (rect.get_height()-4.0f) * scroll_offset / items.size();

      Display::fill_rounded_rect(Rectf(rect.right - 24, rect.top + 2.0f + scrollbar_incr,
                                       rect.right - 2,  rect.top + 2.0f + scrollbar_incr + scrollbar_height),
                                 5.0f,
                                 Color(0.5f, 0.5f, 0.5f, 0.75f));
      
      Display::draw_rounded_rect(Rectf(rect.right - 24, rect.top + 2.0f,
                                       rect.right - 2,  rect.bottom - 2.0f),
                                 5.0f,
                                 Color(1.0f, 1.0f, 1.0f, 1.0f));
    }
  else
    { // all items fit on the screen
      for(Items::size_type i = 0; i < items.size(); ++i)
        {
          items[i]->draw(Rectf(rect.left, rect.top + i * step + 2.0f, 
                               rect.right, rect.top + (i+1) * step - 2.0f), 
                         is_active() && (int(i) == current_item));
        }
    }
}

void
MenuComponent::update(float delta, const Controller& controller)
{
  for(InputEventLst::const_iterator i = controller.get_events().begin(); 
      i != controller.get_events().end(); 
      ++i)
    {
      if (i->type == BUTTON_EVENT && i->button.down)
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
                  sound_manager->play("sounds/menu_click.wav");
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
              sound_manager->play("sounds/menu_change.wav");
              
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
              sound_manager->play("sounds/menu_change.wav");

              if (dynamic_cast<TabComponent*>(parent))
                {
                  current_item = math::mid(0, current_item + 1, static_cast<int>(items.size()-1)); 
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
  return 200; // FIXME:
}

float
MenuComponent::get_prefered_height() const
{
  float step = font->get_height() + 20.0f;
  return step * items.size();
}

float
MenuComponent::calc_height()
{
  return items.size() * item_height();
}

float
MenuComponent::item_height() const
{
  float spacing = 10.0f;
  return font->get_height() + spacing*2.0f;
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

} // namespace gui

/* EOF */
