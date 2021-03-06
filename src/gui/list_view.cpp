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

#include "display/display.hpp"
#include "input/controller.hpp"
#include "font/fonts.hpp"
#include "gui/list_view.hpp"

namespace gui {

ListView::ListView(const Rectf& rect_, Component* parent_)
  : Component(rect_, parent_),
    columns(),
    items(),
    current_item(0)
{
}

ListView::~ListView()
{
}

void
ListView::draw()
{
  TTFFont* font = Fonts::current()->vera20.get();

  float x = rect.left;
  float y = rect.top + static_cast<float>(font->get_height());
  float padding = 10;

  for(int i = 0; i < int(columns.size()); ++i)
  {
    // FIXME: Poor mans outline effect
    font->draw_center(Vector2f(x + columns[i].width/2 + 1, y - 1), columns[i].title);
    font->draw_center(Vector2f(x + columns[i].width/2 - 1, y - 1), columns[i].title);
    font->draw_center(Vector2f(x + columns[i].width/2 + 1, y + 1), columns[i].title);
    font->draw_center(Vector2f(x + columns[i].width/2 - 1, y + 1), columns[i].title);
    font->draw_center(Vector2f(x + columns[i].width/2, y), columns[i].title, Color(0.0f, 0.0f, 0.0f));
    x += columns[i].width;
  }

  for(int j = 0; j < int(items.size()); ++j)
  {
    x = rect.left;

    if (j == current_item)
      Display::fill_rect(Rectf(x, y,
                               rect.right, y + static_cast<float>(font->get_height())),
                         is_active() ? Color(0.5f, 0.5f, 1.0f, 0.8f) : Color(0.5f, 0.5f, 1.0f, 0.3f));

    y += static_cast<float>(font->get_height());

    for(int i = 0; i < int(items[j].columns.size()) && i < int(columns.size()); ++i)
    {
      font->draw(Vector2f(x + padding, y), items[j].columns[i]);

      x += columns[i].width;
    }
  }
}

void
ListView::update(float , const Controller& controller)
{
  for(InputEventLst::const_iterator i = controller.get_events().begin();
      i != controller.get_events().end(); ++i)
  {
    if (i->type == BUTTON_EVENT && i->button.down)
    {
      if (i->button.name == OK_BUTTON)
      {
        // do something with item (scripting callback)
      }
      else if (i->button.name == CANCEL_BUTTON || i->button.name == ESCAPE_BUTTON)
      {
        set_active(false);
      }
    }
    else if (i->type == AXIS_EVENT)
    {
      if (i->axis.name == Y_AXIS)
      {
        if (i->axis.pos > 0)
        {
          if (current_item == int(items.size()) - 1)
            current_item = 0;
          else
            current_item += 1;
        }
        else if (i->axis.pos < 0)
        {
          if (current_item == 0)
            current_item = static_cast<int>(items.size()) - 1;
          else
            current_item -= 1;
        }
      }
    }
  }
}

void
ListView::add_column(const std::string& title, float width)
{
  Column column;

  column.title = title;
  if (width == -1)
    column.width = 150;
  else
    column.width = width;

  columns.push_back(column);
}

void
ListView::add_item(const Item& item)
{
  items.push_back(item);
}

} // namespace gui

/* EOF */
