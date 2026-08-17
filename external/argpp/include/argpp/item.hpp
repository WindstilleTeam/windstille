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

#ifndef HEADER_ARGPARSER_ITEM_HPP
#define HEADER_ARGPARSER_ITEM_HPP

#include <assert.h>
#include <functional>
#include <string_view>
#include <variant>
#include <vector>

#include "flags.hpp"
#include "fwd.hpp"

namespace argparser {

class Item
{
public:
  Item(Flags const& flags) : m_flags(flags)
  {}
  virtual ~Item() = default;

  virtual void print(PrettyPrinter& printer) = 0;
  virtual std::string text() const { return {}; }

  Flags const& get_flags() const { return m_flags; }

protected:
  Flags m_flags; // NOLINT
};

} // namespace argparser

#endif

/* EOF */
