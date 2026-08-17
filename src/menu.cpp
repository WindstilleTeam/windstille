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

#include <functional>

#include "group_component.hpp"
#include "gui_manager.hpp"
#include "menu.hpp"
#include "menu_component.hpp"
#include "menu_item.hpp"
#include "root_component.hpp"
#include "screen_manager.hpp"

namespace wstgui {

Menu::Menu(const std::string& name, const geom::frect& rect, Style& style, bool allow_cancel, Component* parent) :
  m_manager(),
  m_group(),
  m_menu()
{
  if (!parent)
  {
    m_manager.reset(new GUIManager(style));
    parent = m_manager->get_root();
  }

  m_group.reset(new GroupComponent(name, parent));
  m_group->set_geometry(rect);
  m_menu.reset(new MenuComponent(allow_cancel, m_group.get()));
  m_menu->set_geometry(m_group->get_child_rect());
  m_menu->set_font(style.get_font());
}

Menu::~Menu()
{
}

EnumMenuItem&
Menu::add_enum(const std::string& name, int index,
               const std::function<void (int)>& callback)
{
  std::unique_ptr<EnumMenuItem> enum_item(new EnumMenuItem(m_menu.get(), name, index));

  if (callback)
    enum_item->sig_change().connect(callback);

  EnumMenuItem& obj = *enum_item;
  m_menu->add_item(std::move(enum_item));
  return obj;
}

void
Menu::add_slider(const std::string& name,
                 int value, int min_value, int max_value, int step,
                 const std::function<void (int)>& callback)
{
  std::unique_ptr<SliderMenuItem> slider(new SliderMenuItem(m_menu.get(), name, value, min_value, max_value, step));
  if (callback)
    slider->sig_change().connect(callback);
  m_menu->add_item(std::move(slider));
}

void
Menu::add_button(const std::string& name,
                 const std::function<void ()>& callback)
{
  std::unique_ptr<ButtonMenuItem> scenario_button(new ButtonMenuItem(m_menu.get(), name));
  if (callback)
    scenario_button->sig_click().connect(callback);
  m_menu->add_item(std::move(scenario_button));
}

RootComponent*
Menu::get_root() const
{
  assert(m_manager.get());
  return m_manager->get_root();
}

void
Menu::show(ScreenManager& screen_manager)
{
  assert(m_manager.get());

  {
    geom::frect rect = m_group->geometry();

    glm::vec2 center((rect.left() + rect.right()) / 2.0f,
                     (rect.top() + rect.bottom()) / 2.0f);

    geom::fsize size(m_menu->get_prefered_size().width(),
                     m_menu->get_prefered_size().height() +
                     (m_group->has_title() ?
                      static_cast<float>(m_manager->get_style().get_font()->get_height()) + 18.0f :
                      0.0f));

    m_group->set_geometry(geom::frect(glm::vec2(center.x - size.width()/2.0f,
                                                   center.y - size.height()/2.0f),
                                         size));

    m_menu->set_geometry(m_group->get_child_rect());
  }

  m_group->pack(std::move(m_menu));
  m_manager->get_root()->add_child(std::move(m_group));
  screen_manager.push_overlay(std::move(m_manager));
}

} // namespace wstgui

/* EOF */
