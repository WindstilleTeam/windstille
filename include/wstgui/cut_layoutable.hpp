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

#ifndef HEADER_WSTGUI_CUT_LAYOUTABLE_HPP
#define HEADER_WSTGUI_CUT_LAYOUTABLE_HPP

#include <memory>

#include "ilayoutable.hpp"

namespace wstgui {

class CutLayoutable : public ILayoutable
{
public:
  enum class Orientation { Horizontal, Vertical };

public:
  CutLayoutable(Orientation orientation, float length) :
    m_orientation(orientation),
    m_length(length),
    m_lhs(),
    m_rhs()
  {}

  void pack(std::unique_ptr<ILayoutable> layoutable);

  void set_geometry(geom::frect const& rect) override;

private:
  Orientation m_orientation;
  float m_length;
  std::unique_ptr<ILayoutable> m_lhs;
  std::unique_ptr<ILayoutable> m_rhs;
};

} // namespace wstgui

#endif

/* EOF */
