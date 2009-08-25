/*
**  Windstille - A Sci-Fi Action-Adventure Game
**  Copyright (C) 2005 Ingo Ruhnke <grumbel@gmx.de>
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

#ifndef HEADER_WINDSTILLE_GUI_GROUP_COMPONENT_HPP
#define HEADER_WINDSTILLE_GUI_GROUP_COMPONENT_HPP

#include "gui/component.hpp"

namespace gui {

/** */
class GroupComponent : public Component
{
private:
  std::string title;
  boost::scoped_ptr<Component> child;

public:
  GroupComponent(const Rectf& rect, const std::string& title_, Component* parent);
  virtual ~GroupComponent();

  void draw();
  void update(float delta, const Controller& controller);

  void pack(Component* component);
  
  bool is_active() const;

  Rectf get_child_rect() const;

  bool has_title() const { return !title.empty(); }

private:
  GroupComponent (const GroupComponent&);
  GroupComponent& operator= (const GroupComponent&);
};

} // namespace gui

#endif

/* EOF */
