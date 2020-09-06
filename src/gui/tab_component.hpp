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

#ifndef HEADER_WINDSTILLE_GUI_TAB_COMPONENT_HPP
#define HEADER_WINDSTILLE_GUI_TAB_COMPONENT_HPP

#include "gui/component.hpp"

namespace gui {

/** */
class TabComponent : public Component
{
private:
  struct Tab
  {
    std::string label;
    Component*  component;

    Tab()
      : label(),
        component(nullptr)
    {}

    Tab(const std::string& label_, Component* c)
      : label(label_),
        component(c)
    {}

    Tab(const Tab& rhs)
      : label(rhs.label),
        component(rhs.component)
    {}

    Tab& operator=(const Tab& rhs)
    {
      if (this != &rhs)
      {
        label     = rhs.label;
        component = rhs.component;
      }
      return *this;
    }
  };

  typedef std::vector<Tab> Tabs;
  Tabs tabs;

  int current_tab;

public:
  TabComponent(Component* parent);
  TabComponent(const Rectf& rect, Component* parent);
  ~TabComponent() override;

  void draw() override;
  void update(float delta, const Controller& controller) override;

  void pack(const std::string& name, Component* component);

private:
  TabComponent (const TabComponent&);
  TabComponent& operator= (const TabComponent&);
};

} // namespace gui

#endif

/* EOF */
