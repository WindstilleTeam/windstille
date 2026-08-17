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

#ifndef HEADER_WINDSTILLE_GUI_LIST_VIEW_HPP
#define HEADER_WINDSTILLE_GUI_LIST_VIEW_HPP

#include "component.hpp"

namespace wstgui {

/** */
class ListView : public Component
{
private:
  struct Column {
    std::string title;
    float       width;

    Column()
      : title(),
        width()
    {}
  };

public:
  struct Item {
    std::vector<std::string> columns;

    Item(const std::string& el1,
         const std::string& el2,
         const std::string& el3)
      : columns()
    {
      columns.push_back(el1);
      columns.push_back(el2);
      columns.push_back(el3);
    }
  };

private:
  typedef std::vector<Item> Items;
  typedef std::vector<Column> Columns;
  Columns columns;
  Items   items;
  int     current_item;

public:
  ListView(Component* parent);
  ~ListView() override;

  void draw(wstdisplay::GraphicsContext& gc) override;
  void update(float delta, const Controller& controller) override;

  /** if width is -1 it will be automatically spaced */
  void add_column(const std::string& name, float width = -1);
  void add_item(const Item& item);

private:
  ListView (const ListView&);
  ListView& operator= (const ListView&);
};

} // namespace wstgui

#endif

/* EOF */
