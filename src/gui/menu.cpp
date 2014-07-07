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

#include "gui/root_component.hpp"
#include "gui/gui_manager.hpp"
#include "gui/menu_item.hpp"
#include "gui/menu_component.hpp"
#include "gui/group_component.hpp"
#include "screen/screen_manager.hpp"
#include "gui/menu.hpp"

namespace gui {

Menu::Menu(const std::string& name, const Rectf& rect, bool allow_cancel, Component* parent)
  : manager(),
    group(),
    menu()
{
  if (!parent)
  {
    manager.reset(new GUIManager());
    parent = manager->get_root();
  }  

  group.reset(new GroupComponent(rect, name, parent));
  menu.reset(new MenuComponent(group->get_child_rect(), allow_cancel, group.get()));
  menu->set_font(Fonts::current()->vera20.get());
}

Menu::~Menu()
{
}

EnumMenuItem&
Menu::add_enum(const std::string& name, int index,
               const boost::function<void (int)>& callback)
{
  std::unique_ptr<EnumMenuItem> enum_item(new EnumMenuItem(menu.get(), name, index));

  if (callback)
    enum_item->sig_change().connect(callback);

  EnumMenuItem& obj = *enum_item;
  menu->add_item(enum_item.release());
  return obj;
}

void
Menu::add_slider(const std::string& name, 
                 int value, int min_value, int max_value, int step,
                 const boost::function<void (int)>& callback)
{
  std::unique_ptr<SliderMenuItem> slider(new SliderMenuItem(menu.get(), name, value, min_value, max_value, step));
  if (callback)
    slider->sig_change().connect(callback);
  menu->add_item(slider.release());
}

void
Menu::add_button(const std::string& name,
                 const boost::function<void ()>& callback)
{
  std::unique_ptr<ButtonMenuItem> scenario_button(new ButtonMenuItem(menu.get(), name));
  if (callback)
    scenario_button->sig_click().connect(callback);
  menu->add_item(scenario_button.release());
}

RootComponent*
Menu::get_root() const
{
  assert(manager.get());
  return manager->get_root();
}

void
Menu::show()
{
  assert(manager.get());
  
  {
    Rectf rect = group->get_screen_rect();

    Vector2f center((rect.left + rect.right) / 2.0f,
                    (rect.top + rect.bottom) / 2.0f);

    Sizef size(menu->get_prefered_width(), 
               menu->get_prefered_height() + (group->has_title() ? static_cast<float>(Fonts::current()->vera20->get_height()) + 18.0f : 0.0f));

    group->set_screen_rect(Rectf(Vector2f(center.x - size.width/2.0f,
                                          center.y - size.height/2.0f),
                                 size));

    menu->set_screen_rect(group->get_child_rect());
  }

  group->pack(menu.release());
  manager->get_root()->add_child(group.release());
  ScreenManager::current()->push_overlay(manager.release());
}

} // namespace gui

/* EOF */
