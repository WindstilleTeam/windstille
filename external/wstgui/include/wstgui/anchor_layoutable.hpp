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

#ifndef HEADER_WSTGUI_ANCHOR_LAYOUTABLE_HPP
#define HEADER_WSTGUI_ANCHOR_LAYOUTABLE_HPP

#include <memory>
#include <vector>

#include "rfloat.hpp"
#include "ilayoutable.hpp"

namespace wstgui {

/** Layoutable that allows placing components either at an absolute
    positions or relative to it's parent size */
class AnchorLayoutable : public ILayoutable
{
public:
  AnchorLayoutable();

  void set_geometry(geom::frect const& rect) override;

  void place(std::unique_ptr<ILayoutable> layoutable, rfloat x, rfloat y, rfloat w, rfloat h, geom::origin origin);
  void place(Component* component, rfloat x, rfloat y, rfloat w, rfloat h, geom::origin origin);

  void place(std::unique_ptr<ILayoutable> layoutable, geom::frect const& rect);
  void place(Component* component, geom::frect const& rect);

private:
  struct Item
  {
    std::unique_ptr<ILayoutable> layoutable;
    rfloat x;
    rfloat y;
    rfloat width;
    rfloat height;
    geom::origin origin;
  };

private:
  std::vector<Item> m_items;

private:
  AnchorLayoutable(const AnchorLayoutable&) = delete;
  AnchorLayoutable& operator=(const AnchorLayoutable&) = delete;
};

} // namespace wstgui

#endif

/* EOF */
