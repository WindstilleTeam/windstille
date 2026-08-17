/*
**  Windstille - A Sci-Fi Action-Adventure Game
**  Copyright (C) 2009 Ingo Ruhnke <grumbel@gmail.com>
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

#include <functional>

#include "fwd.hpp"

namespace wstgui {

/**
 *  Little helper class around MenuItem and MenuComponent to reduce code clutter.
 */
class Menu
{
public:
  /**
   *  Construct a Menu, if \a parent is given no GUIManager will be
   *  created and show() will not work, use create_group() instead and
   *  add the result to your parent component.
   */
  Menu(const std::string& name, const geom::frect& rect, Style& style,
       bool allow_cancel = true, Component* parent = nullptr);
  ~Menu();

  EnumMenuItem& add_enum(const std::string& name,
                         int index,
                         const std::function<void (int)>& callback = std::function<void (int)>());

  void add_slider(const std::string& name,
                  int value, int mix_value, int max_value, int step,
                  const std::function<void (int)>& callback = std::function<void (int)>());

  void add_button(const std::string& name,
                  const std::function<void ()>& callback = std::function<void ()>());

  RootComponent*  get_root() const;

  void show(ScreenManager& screen_manager);

private:
  std::unique_ptr<GUIManager> m_manager;
  std::unique_ptr<GroupComponent> m_group;
  std::unique_ptr<MenuComponent> m_menu;

private:
  Menu (const Menu&);
  Menu& operator= (const Menu&);
};

} // namespace wstgui

#endif

/* EOF */
