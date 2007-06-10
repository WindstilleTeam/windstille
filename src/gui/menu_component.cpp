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
#include "input/controller.hpp"
#include "menu_component.hpp"
#include "display/display.hpp"
#include "math.hpp"

namespace GUI {

MenuItem::MenuItem(MenuComponent* parent_, const std::string& label_)
  : parent(parent_),
    label(label_)
{}

void
MenuItem::draw(const Rectf& rect, bool is_active)
{
  Color font_color;
  TTFFont* font = Fonts::vera16;
  
  if (is_active) {
    Display::fill_rounded_rect(rect, 5.0f, Color(0.5f, 0.5f, 0.5f, 0.75f));
    Display::draw_rounded_rect(rect, 5.0f, Color(1.0f, 1.0f, 1.0f, 1.0f));
    font_color = Color(1.0f, 1.0f, 1.0f);
  } else {
    Display::fill_rounded_rect(rect, 5.0f, Color(0.3f, 0.3f, 0.3f, 0.75f));
    Display::draw_rounded_rect(rect, 5.0f, Color(1.0f, 1.0f, 1.0f, 0.75f));
    font_color = Color(0.75f, 0.75f, 0.75f, 1.0f);
  }

  font->draw(rect.left + font->get_height(), rect.top + font->get_height()/2.0f + rect.get_height()/2.0f - 2.0f,
             label, font_color);
      
}

void
MenuItem::update(float delta)
{
}

EnumMenuItem::EnumMenuItem(MenuComponent* parent_, 
                           const std::string& label_, int index_)
  : MenuItem(parent_, label_),
    index(index_)
{
}

void
EnumMenuItem::add_pair(int value, const std::string& label)
{
  EnumValue enum_value;
  enum_value.value = value;
  enum_value.label = label;
  labels.push_back(enum_value);
}

void
EnumMenuItem::incr()
{
  index -= 1;
  if (index < 0)
    index = labels.size()-1;
  on_change(labels[index].value);
}

void
EnumMenuItem::decr()
{
  index += 1;
  if (index >= static_cast<int>(labels.size()))
    index = 0;
  on_change(labels[index].value);
}

void 
EnumMenuItem::draw(const Rectf& rect, bool is_active)
{
  MenuItem::draw(rect, is_active);
  TTFFont* font = Fonts::vera16;
  Color font_color;
  if (is_active)
    {
      font_color = Color(1.0f, 1.0f, 1.0f);
    }
  else
    {
      font_color = Color(0.75f, 0.75f, 0.75f, 1.0f);
    }

  font->draw(rect.right - font->get_height() - font->get_width("< " + labels[index].label + " >"),
             rect.top + font->get_height()/2.0f + rect.get_height()/2.0f - 2.0f,
             "< " + labels[index].label + " >",
             font_color);
}

void 
EnumMenuItem::update(float delta)
{
  MenuItem::update(delta);
}

SliderMenuItem::SliderMenuItem(MenuComponent* parent_, 
                               const std::string& label_,
                               int value_, int min_value_, int max_value_, int step_) 
  : MenuItem(parent_, label_),
    value(value_),
    min_value(min_value_),
    max_value(max_value_),
    step(step_)
{  
}

void
SliderMenuItem::incr()
{
  value += step;
  if (value > max_value)
    value = max_value;
  on_change(value);
}

void
SliderMenuItem::decr()
{
  value -= step;
  if (value < min_value)
    value = min_value;
  on_change(value);
}

void
SliderMenuItem::draw(const Rectf& rect, bool is_active)
{
  MenuItem::draw(rect, is_active);
  int total_width = 200;
  int width = total_width * value / (max_value - min_value);

  Color color;
  if (is_active)
    {
      color = Color(1.0f, 1.0f, 1.0f);
    }
  else
    {
      color = Color(0.75f, 0.75f, 0.75f, 1.0f);
    }

  Display::fill_rounded_rect(Rectf(Vector(rect.right - 4 - width, rect.top + 4),
                                   Sizef(width, rect.get_height() - 8)), 
                             5.0f,
                             Color(0.75f*color.r, 0.75f*color.g, 0.75f*color.b, color.a));


  Display::draw_rounded_rect(Rectf(Vector(rect.right - 4 - total_width, rect.top + 4),
                                   Sizef(total_width, rect.get_height() - 8)), 
                             5.0f,
                             color);
}

void
SliderMenuItem::update(float delta)
{
  MenuItem::update(delta);
}

MenuComponent::MenuComponent(const Rectf& rect, Component* parent)
  : Component(rect, parent),
    current_item(0)
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
}

void
MenuComponent::draw()
{
  float spacing = 10.0f;
  float step = Fonts::vera16->get_height() + spacing*2.0f;

  for(Items::size_type i = 0; i < items.size(); ++i)
    {
      items[i]->draw(Rectf(rect.left, rect.top + i * step + 2.0f, 
                           rect.right, rect.top + (i+1) * step - 2.0f), 
                     is_active() && (int(i) == current_item));
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
          if (i->button.name == OK_BUTTON)
            {
              
            }
          else if (i->button.name == CANCEL_BUTTON)
            {
              set_active(false);
            }
        }
      else if (i->type == AXIS_EVENT)
        {
          if (i->axis.name == X_AXIS)
            {
              if (i->axis.pos < 0)
                {
                  items[current_item]->incr();
                  // insert callback here
                }
              else if (i->axis.pos > 0)
                {
                  items[current_item]->decr();
                  // insert callback here
                }
            }
          else if (i->axis.name == Y_AXIS)
            {
              if (i->axis.pos < 0)
                {
                  current_item = current_item - 1;
                  if (current_item < 0)
                    {
                      current_item = 0;
                      set_active(false);
                    }
                }
              else if (i->axis.pos > 0)
                {
                  current_item = Math::mid(0, current_item + 1, static_cast<int>(items.size()-1)); 
                }
            }
        }
    }
}

} // namespace GUI

/* EOF */
