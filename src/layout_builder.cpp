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

#include "layout_builder.hpp"

#include <logmich/log.hpp>

#include "cut_layoutable.hpp"

namespace wstgui {

LayoutBuilder::LayoutBuilder() :
  m_stack(),
  m_root()
{
}

LayoutBuilder&
LayoutBuilder::begin_hcut(float length)
{
  m_stack.emplace_back(std::make_unique<CutLayoutable>(CutLayoutable::Orientation::Horizontal, length));
  return *this;
}

LayoutBuilder&
LayoutBuilder::begin_vcut(float length)
{
  m_stack.emplace_back(std::make_unique<CutLayoutable>(CutLayoutable::Orientation::Vertical, length));
  return *this;
}

LayoutBuilder&
LayoutBuilder::next()
{
  log_not_implemented();

  return *this;
}

LayoutBuilder&
LayoutBuilder::pack(Component* component)
{
  pack(std::make_unique<LayoutableComponent>(component));

  return *this;
}

LayoutBuilder&
LayoutBuilder::pack(std::unique_ptr<ILayoutable> layoutable)
{
  if (m_stack.empty()) {
    throw std::runtime_error("LayoutBuilder stack empty");
  }

  m_stack.back()->pack(std::move(layoutable));

  return *this;
}

LayoutBuilder&
LayoutBuilder::end()
{
  std::unique_ptr<CutLayoutable> cutable = std::move(m_stack.back());
  m_stack.pop_back();

  if (!m_stack.empty()) {
    m_stack.back()->pack(std::move(cutable));
  } else {
    m_root = std::move(cutable);
  }

  return *this;
}

std::unique_ptr<ILayoutable>
LayoutBuilder::finalize()
{
  if (!m_root) {
    throw std::runtime_error("LayoutBuilder not finished");
  }

  return std::move(m_root);
}

} // namespace wstgui

/* EOF */
