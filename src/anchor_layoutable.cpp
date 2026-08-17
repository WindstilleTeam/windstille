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

#include "anchor_layoutable.hpp"

#include <logmich/log.hpp>
#include <geom/io.hpp>

namespace wstgui {

AnchorLayoutable::AnchorLayoutable() :
  m_items()
{
}

void
AnchorLayoutable::set_geometry(geom::frect const& geometry)
{
  for (auto const& item : m_items)
  {
    geom::frect rect = geom::anchored_rect(geom::fpoint(item.x(geometry.width()),
                                                  item.y(geometry.height())),
                                           geom::fsize(item.width(geometry.width()),
                                                       item.height(geometry.height())),
                                           item.origin);
    item.layoutable->set_geometry(rect);
  }
}

void
AnchorLayoutable::place(std::unique_ptr<ILayoutable> layoutable,
                        rfloat x, rfloat y, rfloat w, rfloat h, geom::origin origin)
{
  m_items.emplace_back(Item{std::move(layoutable), x, y, w, h, origin});
}

void
AnchorLayoutable::place(Component* component,
                        rfloat x, rfloat y, rfloat w, rfloat h, geom::origin origin)
{
  place(std::make_unique<LayoutableComponent>(component), x, y, w, h, origin);
}

void
AnchorLayoutable::place(std::unique_ptr<ILayoutable> layoutable, geom::frect const& rect)
{
  place(std::move(layoutable),
        rfloat::absolute(rect.x()), rfloat::absolute(rect.y()),
        rfloat::absolute(rect.size().width()), rfloat::absolute(rect.size().height()),
        geom::origin::TOP_LEFT);
}

void
AnchorLayoutable::place(Component* component, geom::frect const& rect)
{
  place(std::make_unique<LayoutableComponent>(component), rect);
}

} // namespace wstgui

/* EOF */
