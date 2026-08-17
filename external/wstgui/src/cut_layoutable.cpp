/*
**  Windstille - A Sci-Fi Action-Adventure Game
**  Copyright (C) 2018 Ingo Ruhnke <grumbel@gmail.com>
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

#include "cut_layoutable.hpp"

#include <logmich/log.hpp>

namespace wstgui {

void
CutLayoutable::pack(std::unique_ptr<ILayoutable> layoutable)
{
  if (!m_lhs) {
    m_lhs = std::move(layoutable);
  } else if (!m_rhs) {
    m_rhs = std::move(layoutable);
  } else {
     throw std::runtime_error("CutLayoutable already filled");
  }
}

void
CutLayoutable::set_geometry(geom::frect const& rect)
{
  if (m_orientation == Orientation::Horizontal)
  {
    if (m_lhs) {
      m_lhs->set_geometry({
          rect.left(),
          rect.top(),
          rect.left() + m_length,
          rect.bottom(),
        });
    }

    if (m_rhs) {
      m_rhs->set_geometry({
          rect.left() + m_length,
          rect.top(),
          rect.right(),
          rect.bottom(),
        });
    }
  }
  else // if (m_orientation == Orientation::Vertical)
  {
    if (m_lhs) {
      m_lhs->set_geometry({
          rect.left(),
          rect.top(),
          rect.right(),
          rect.top() + m_length,
        });
    }

    if (m_rhs) {
      m_rhs->set_geometry({
          rect.left(),
          rect.top() + m_length,
          rect.right(),
          rect.bottom(),
        });
    }
  }
}

} // namespace wstgui

/* EOF */
