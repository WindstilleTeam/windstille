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

#ifndef HEADER_WSTGUI_BOX_LAYOUTABLE_HPP
#define HEADER_WSTGUI_BOX_LAYOUTABLE_HPP

#include <memory>
#include <vector>

#include "ilayoutable.hpp"

namespace wstgui {

class BoxLayoutable : public ILayoutable
{
public:
  enum class Orientation { Horizontal, Vertical };

public:
  BoxLayoutable(Orientation orientation);

  void pack(std::unique_ptr<ILayoutable> layoutable, bool expand = true, bool fill = true, float margin = 0.0f);
  void pack(Component* component, bool expand = true, bool fill = true, float margin = 0.0f);
  void pack_spacer(float length);
  void pack_stretcher();

  void set_geometry(geom::frect const& rect) override;

private:
  struct Item {
    std::unique_ptr<ILayoutable> layoutable;
    bool expand = false;
    bool fill = true;
    float margin = 0.0f;
  };

private:
  Orientation m_orientation;
  std::vector<Item> m_items;

private:
  BoxLayoutable(const BoxLayoutable&) = delete;
  BoxLayoutable& operator=(const BoxLayoutable&) = delete;
};

class HBox : public BoxLayoutable
{
public:
  HBox() :
    BoxLayoutable(Orientation::Horizontal)
  {}
};

class VBox : public BoxLayoutable
{
public:
  VBox() :
    BoxLayoutable(Orientation::Vertical)
  {}
};

} // namespace wstgui

#endif

/* EOF */
