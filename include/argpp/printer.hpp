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

#ifndef HEADER_ARGPARSER_PRINTER_HPP
#define HEADER_ARGPARSER_PRINTER_HPP

#include <iostream>
#include <stdint.h>

#include "fwd.hpp"

namespace argparser {

class Printer
{
public:
  Printer(OptionGroup const& options);

  void print_usage(std::ostream& out = std::cout) const;
  void print_help(std::ostream& out = std::cout, uint32_t visibility_mask = ~0) const;
  void print_help(CommandItem const& command_item, std::ostream& out = std::cout, uint32_t visibility_mask = ~0) const;

private:
  void print_help(OptionGroup const& group, CommandItem const* current_command_item, std::ostream& out, uint32_t visibility_mask, bool skip_print_usage) const;
  void print_usage(CommandItem const* current_command_item, std::ostream& out) const;

private:
  OptionGroup const& m_options;
};

} // namespace argparser

#endif

/* EOF */
