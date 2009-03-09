/*
**  Windstille - A Sci-Fi Action-Adventure Game
**  Copyright (C) 2009 Ingo Ruhnke <grumbel@gmx.de>
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

#ifndef HEADER_WINDSTILLE_GUI_MENU_HPP
#define HEADER_WINDSTILLE_GUI_MENU_HPP

#include <memory>

namespace gui {

class GUIManager;
class Component;
class GroupComponent;
class MenuComponent;
class EnumMenuItem;

/** 
 *  Little helper class around MenuItem and MenuComponent to reduce code clutter.
 */
class Menu
{
private:
  std::auto_ptr<GUIManager>     manager;
  std::auto_ptr<GroupComponent> group;
  std::auto_ptr<MenuComponent>  menu;

public:
  Menu(const std::string& name, const Rectf& rect);
  ~Menu();

  EnumMenuItem& add_enum(const std::string& name,  
                         int index,
                         const boost::function<void (int)>& callback);

  void  add_slider(const std::string& name, 
                   int value, int mix_value, int max_value, int step,
                   const boost::function<void (int)>& callback);

  void  add_button(const std::string& name,
                   const boost::function<void ()>& callback);

  void push_screen();

private:
  Menu (const Menu&);
  Menu& operator= (const Menu&);
};

} // namespace gui

#endif

/* EOF */
