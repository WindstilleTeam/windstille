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

#include "argparser.hpp"

#include "printer.hpp"

namespace argparser {

void parse(OptionGroup const& options, int argc, char const* const* argv)
{
  parse(options, std::span<char const* const>(argv, argc));
}

void parse(OptionGroup const& options, std::span<char const* const> argv)
{
  Parser parser(options);
  parser.parse_args(argv);
}

void print_help(OptionGroup const& options, std::ostream& out, uint32_t visibility_mask)
{
  Printer printer(options);
  printer.print_help(out, visibility_mask);
}

void print_help(OptionGroup const& options, CommandItem const& command_item, std::ostream& out, uint32_t visibility_mask)
{
  Printer printer(options);
  printer.print_help(command_item, out, visibility_mask);
}

} // namespace argparser

/* EOF */
