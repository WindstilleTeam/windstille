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

#ifndef HEADER_WINDSTILLE_GUI_LIST_VIEW_HPP
#define HEADER_WINDSTILLE_GUI_LIST_VIEW_HPP

#include <vector>
#include <string>
#include "component.hpp"

namespace gui { 

/** */
class ListView : public Component
{
private:
  struct Column {
    std::string title;
    float       width;
  };
  
public:
  struct Item {
    std::vector<std::string> columns;

    Item(const std::string& el1, 
         const std::string& el2,
         const std::string& el3)
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
  ListView(const Rectf& rect, Component* parent);
  ~ListView();
  
  void draw();
  void update(float delta, const Controller& controller);

  /** if width is -1 it will be automatically spaced */
  void add_column(const std::string& name, float width = -1);
  void add_item(const Item& item);
  
private:
  ListView (const ListView&);
  ListView& operator= (const ListView&);
};

} // namespace gui 

#endif

/* EOF */
