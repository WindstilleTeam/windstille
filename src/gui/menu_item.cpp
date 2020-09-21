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

#include "gui/menu_item.hpp"

#include "app/app.hpp"
#include "app/sound_manager.hpp"
#include "display/display.hpp"
#include "gui/menu_component.hpp"

namespace gui {

MenuItem::MenuItem(MenuComponent* parent_, const std::string& label_)
  : parent(parent_),
    label(label_),
    fade_timer(0.0f)
{}

void
MenuItem::draw(const Rectf& rect, bool is_active)
{
  Color font_color;
  TTFFont* font = parent->get_font();

  if (is_active)
  {
    Display::fill_rounded_rect(rect, 5.0f, Color(0.5f, 0.5f, 0.5f, 0.75f));
    Display::draw_rounded_rect(rect, 5.0f, Color(1.0f, 1.0f, 1.0f, 1.0f));
    font_color = Color(1.0f, 1.0f, 1.0f);
    fade_timer = 2.0f;
  }
  else
  {
    if (fade_timer != 0.0f)
    {
      //Display::fill_rounded_rect(rect, 5.0f, Color(0.5f, 0.5f, 0.5f, 0.75f * fade_timer));
      //Display::draw_rounded_rect(rect, 5.0f, Color(1.0f, 1.0f, 1.0f, 1.0f * fade_timer));
      font_color = Color(0.75f + 0.25f * fade_timer,
                         0.75f + 0.25f * fade_timer,
                         0.75f + 0.25f * fade_timer,
                         1.0f);
    }
    else
    {
      font_color = Color(0.75f, 0.75f, 0.75f, 1.0f);
    }
  }

  font->draw(Vector2f(rect.left() + static_cast<float>(font->get_height()),
                      rect.top() + static_cast<float>(font->get_height())/2.0f + rect.height() / 2.0f - 2.0f),
             label, font_color);
}

void
MenuItem::update(float delta)
{
  if (fade_timer > 0.0f)
    fade_timer -= delta / 0.25f;
  else
    fade_timer = 0.0f;
}

EnumMenuItem::EnumMenuItem(MenuComponent* parent_,
                           const std::string& label_, int index_)
  : MenuItem(parent_, label_),
    index(index_),
    labels(),
    on_change()
{
}

EnumMenuItem&
EnumMenuItem::add_pair(int value_, const std::string& label_)
{
  EnumValue enum_value;
  enum_value.value = value_;
  enum_value.label = label_;
  labels.push_back(enum_value);

  return *this;
}

void
EnumMenuItem::incr()
{
  g_app.sound().play(Pathname("sounds/menu_click.wav", Pathname::kDataPath));

  index -= 1;
  if (index < 0)
    index = static_cast<int>(labels.size()) - 1;
  on_change(labels[index].value);
}

void
EnumMenuItem::decr()
{
  g_app.sound().play(Pathname("sounds/menu_click.wav", Pathname::kDataPath));

  index += 1;
  if (index >= static_cast<int>(labels.size()))
    index = 0;
  on_change(labels[index].value);
}

void
EnumMenuItem::draw(const Rectf& rect, bool is_active)
{
  MenuItem::draw(rect, is_active);
  TTFFont* font = parent->get_font();
  Color font_color;
  if (is_active)
  {
    font_color = Color(1.0f, 1.0f, 1.0f);
  }
  else
  {
    font_color = Color(0.75f, 0.75f, 0.75f, 1.0f);
  }

  font->draw(Vector2f(rect.right() - static_cast<float>(font->get_height())      - static_cast<float>(font->get_width(labels[index].label)),
                      rect.top()   + static_cast<float>(font->get_height())/2.0f + rect.height() / 2.0f - 2.0f),
             labels[index].label,
             font_color);
}

SliderMenuItem::SliderMenuItem(MenuComponent* parent_,
                               const std::string& label_,
                               int value_, int min_value_, int max_value_, int step_)
  : MenuItem(parent_, label_),
    value(value_),
    min_value(min_value_),
    max_value(max_value_),
    step(step_),
    on_change()
{
}

void
SliderMenuItem::decr()
{
  g_app.sound().play(Pathname("sounds/menu_click.wav", Pathname::kDataPath));

  value += step;
  if (value > max_value)
    value = max_value;
  on_change(value);
}

void
SliderMenuItem::incr()
{
  g_app.sound().play(Pathname("sounds/menu_click.wav", Pathname::kDataPath));

  value -= step;
  if (value < min_value)
    value = min_value;
  on_change(value);
}

void
SliderMenuItem::draw(const Rectf& rect, bool is_active)
{
  MenuItem::draw(rect, is_active);
  float total_width = 200.0f;
  float width = total_width * static_cast<float>(value) / static_cast<float>(max_value - min_value);

  Color color;
  if (is_active)
  {
    color = Color(1.0f, 1.0f, 1.0f);
  }
  else
  {
    color = Color(0.75f, 0.75f, 0.75f, 1.0f);
  }

  Display::fill_rounded_rect(Rectf(Vector2f(rect.right() - 4.0f - total_width, rect.top() + 4.0f),
                                   Sizef(width, rect.height() - 8)),
                             5.0f,
                             Color(0.75f*color.r, 0.75f*color.g, 0.75f*color.b, color.a));

  Display::draw_rounded_rect(Rectf(Vector2f(rect.right() - 4.0f - total_width, rect.top() + 4.0f),
                                   Sizef(total_width, rect.height() - 8)),
                             5.0f,
                             color);
}

ButtonMenuItem::ButtonMenuItem(MenuComponent* parent_, const std::string& label_)
  : MenuItem(parent_, label_),
    on_click()
{
}

void
ButtonMenuItem::click()
{
  g_app.sound().play(Pathname("sounds/menu_click.wav", Pathname::kDataPath));

  on_click();
}

void
ButtonMenuItem::draw(const Rectf& rect, bool is_active)
{
  MenuItem::draw(rect, is_active);
}

} // namespace gui

/* EOF */
