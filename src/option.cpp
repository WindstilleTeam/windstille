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

#include "option.hpp"
#include "prettyprinter.hpp"

namespace argparser {

std::string
Option::text() const
{
  if (m_short_name) {
    return fmt::format("-{}", *m_short_name);
  } else if (m_long_name) {
    return fmt::format("--{}", *m_long_name);
  } else {
    return {};
  }
}

void
Option::print(PrettyPrinter& pprinter)
{
  std::string left_column("  ");
  if (get_short_name() && get_long_name()) {
    left_column += fmt::format("-{}, --{}", *get_short_name(), *get_long_name());
  } else if (get_short_name()) {
    left_column += fmt::format("-{}", *get_short_name());
  } else /* if (get_long_name()) */ {
    left_column += fmt::format("--{}", *get_long_name());
  }

  if (requires_argument()) {
    left_column += fmt::format("{}{}",
                               get_long_name() ? '=' : ' ',
                               dynamic_cast<OptionWithArg&>(*this).get_argument_name());
  }
  left_column += " ";

  pprinter.print(left_column, get_help());
}

} // namespace argparser

/* EOF */
