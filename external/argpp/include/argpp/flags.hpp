// argpp - A Command Line Argument Parser for C++
// Copyright (C) 2008-2020 Ingo Ruhnke <grumbel@gmail.com>
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.

#ifndef HEADER_ARGPARSER_FLAGS_HPP
#define HEADER_ARGPARSER_FLAGS_HPP

#include <stdint.h>

namespace argparser {

class Flags
{
public:
  Flags() :
    m_required(false),
    m_visibility_mask(1),
    m_mutual_exclusion_mask(0)
  {}

  Flags& required() { m_required = true; return *this; }
  Flags& visibility(uint32_t mask) { m_visibility_mask = mask; return *this; }
  Flags mutual_exclusion(uint32_t mask) { m_mutual_exclusion_mask = mask; return *this; }

  bool is_required() const { return m_required; }
  uint32_t get_visibility() const { return m_visibility_mask; }
  uint32_t get_mutual_exclusion() const { return m_mutual_exclusion_mask; }

private:
  bool m_required;
  uint32_t m_visibility_mask;
  uint32_t m_mutual_exclusion_mask;
};

} // namespace argparser

#endif

/* EOF */
