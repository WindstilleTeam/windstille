/*
**  Windstille - A Sci-Fi Action-Adventure Game
**  Copyright (C) 2021 Ingo Ruhnke <grumbel@gmail.com>
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

#ifndef HEADER_WSTGUI_LAYOUT_BUILDER_HPP
#define HEADER_WSTGUI_LAYOUT_BUILDER_HPP

#include <memory>
#include <vector>

#include "ilayoutable.hpp"
#include "cut_layoutable.hpp"

namespace wstgui {

class Component;

class LayoutBuilder
{
public:
  LayoutBuilder();

  LayoutBuilder& begin_hcut(float length);
  LayoutBuilder& begin_vcut(float length);
  LayoutBuilder& next();
  LayoutBuilder& pack(Component* component);
  LayoutBuilder& pack(std::unique_ptr<ILayoutable> layoutable);
  LayoutBuilder& end();

  std::unique_ptr<ILayoutable> finalize();

private:
  std::vector<std::unique_ptr<CutLayoutable>> m_stack;
  std::unique_ptr<ILayoutable> m_root;

private:
  LayoutBuilder(const LayoutBuilder&) = delete;
  LayoutBuilder& operator=(const LayoutBuilder&) = delete;
};

} // namespace wstgui

#endif

/* EOF */
