/*  $Id$
**   __      __ __             ___        __   __ __   __
**  /  \    /  \__| ____    __| _/_______/  |_|__|  | |  |   ____
**  \   \/\/   /  |/    \  / __ |/  ___/\   __\  |  | |  | _/ __ \
**   \        /|  |   |  \/ /_/ |\___ \  |  | |  |  |_|  |_\  ___/
**    \__/\  / |__|___|  /\____ /____  > |__| |__|____/____/\___  >
**         \/          \/      \/    \/                         \/
**  Copyright (C) 2007 Ingo Ruhnke <grumbel@gmx.de>
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

#ifndef HEADER_WINDSTILLE_GUI_MENU_COMPONENT_HPP
#define HEADER_WINDSTILLE_GUI_MENU_COMPONENT_HPP

#include <string>
#include <vector>
#include "font/fonts.hpp"
#include "component.hpp"

class TTFFont;

namespace gui {

class MenuItem;

/** */
class MenuComponent : public Component
{
private:
  typedef std::vector<MenuItem* > Items;
  Items items;

  int   current_item;
  TTFFont* font;
  bool allow_cancel;

  bool scroll_mode;
  int  scroll_offset;
  int  num_displayable_items;

  /** Calculate how much height will be needed for the menu */
  float calc_height();

  /** Return the height of a single item */
  float item_height() const;

  void adjust_scroll_offset();
public:
  MenuComponent(const Rectf& rect, bool allow_cancel_, Component* parent);
  virtual ~MenuComponent();

  void add_item(MenuItem* item);
  void draw();
  void update(float delta, const Controller& controller);

  float get_prefered_width() const;
  float get_prefered_height() const;

  void     set_font(TTFFont* font_);
  TTFFont* get_font();
private:
  MenuComponent (const MenuComponent&);
  MenuComponent& operator= (const MenuComponent&);
};

} // namespace gui

#endif

/* EOF */
