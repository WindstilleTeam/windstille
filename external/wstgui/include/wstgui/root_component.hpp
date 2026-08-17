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

#ifndef HEADER_WINDSTILLE_GUI_ROOT_COMPONENT_HPP
#define HEADER_WINDSTILLE_GUI_ROOT_COMPONENT_HPP

#include <memory>
#include <vector>

#include "component.hpp"
#include "style.hpp"

namespace wstgui {

class RootComponent : public Component
{
public:
  RootComponent(Style& style);
  ~RootComponent() override;

  void draw(wstdisplay::GraphicsContext& gc) override;
  void update(float delta, const Controller& controller) override;

  /** Set the chidren that shall recieve input */
  void set_focus(Component* child);

  void add_child(std::unique_ptr<Component> child);

  Component* get_focus() const { return m_focus; }

  Component* query(geom::fpoint const& pos) const;

  bool is_active() const override;

  Style& get_style() const override;

private:
  Component* m_focus;
  std::vector<std::unique_ptr<Component>> m_children;
  Style& m_style;

private:
  RootComponent (const RootComponent&) = delete;
  RootComponent& operator= (const RootComponent&) = delete;
};

} // namespace wstgui

#endif

/* EOF */
