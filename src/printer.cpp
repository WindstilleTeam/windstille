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

#include "printer.hpp"

#include <sys/ioctl.h>

#include "command_item.hpp"
#include "option_group.hpp"
#include "prettyprinter.hpp"
#include "rest_options_item.hpp"

namespace argparser {

namespace {

constexpr int max_column_width = 5;
constexpr int default_terminal_width = 80;

int get_terminal_width()
{
  struct winsize w;
  if (ioctl(0, TIOCGWINSZ, &w) < 0)
  {
    return default_terminal_width;
  }
  else
  {
    return w.ws_col;
  }
}

} // namespace

Printer::Printer(OptionGroup const& options) :
  m_options(options)
{
}

void
Printer::print_usage(std::ostream& out) const
{
  print_usage(nullptr, out);
}

void
Printer::print_usage(CommandItem const* current_command_item, std::ostream& out) const
{
  auto print_group = [&](OptionGroup const& group) {
    if (group.has_options()) {
      out << " [OPTION]...";
    }

    for (auto const& item : group.get_items()) {
      if (auto* positional_item = dynamic_cast<PositionalItem*>(item.get())) {
        if (positional_item->get_flags().is_required()) {
          out << " " << positional_item->get_name();
        } else {
          out << " [" << positional_item->get_name() << "]";
        }
      }
    }

    for (auto const& item : group.get_items()) {
      if (auto* rest_item = dynamic_cast<RestItem*>(item.get())) {
        if (rest_item->get_flags().is_required()) {
          out << " " << rest_item->get_name() << "...";
        } else {
          out << " [" << rest_item->get_name() << "]...";
        }
      } else if (auto* rest_options_item = dynamic_cast<RestOptionsItem*>(item.get())) {
        out << " (";

        out << " " << rest_options_item->get_name();

        if (rest_options_item->get_options().has_options()) {
          out << " [OPTION]...";
        }

        out << " )...";
      }
    }
  };

  if (m_options.get_usage()) {
    int i = 0;
    for (std::string const& usage : *m_options.get_usage()) {
      if (i == 0) {
        out << "Usage: " << m_options.get_program() << ' ' << usage;
      } else {
        out << "\n       " << m_options.get_program() << ' ' << usage;
      }
      i += 1;
    }
  }
  else if (!m_options.has_commands())
  {
    out << "Usage: " << m_options.get_program();
    print_group(m_options);
  }
  else
  {
    size_t item_idx = 0;
    for (auto const& item : m_options.get_items()) {
      if (auto* command_item = dynamic_cast<CommandItem*>(item.get())) {
        if (current_command_item != nullptr &&
            current_command_item != command_item) {
          continue;
        }

        if (item_idx == 0) {
          out << "Usage: ";
        } else {
          out << "\n       ";
        }

        out << m_options.get_program();

        if (m_options.has_options()) {
          out << " [OPTION]...";
        }

        out << " " << command_item->get_name();

        print_group(command_item->get_options());

        item_idx += 1;
      }
    }
  }

  out << std::endl;
}

void
Printer::print_help(CommandItem const& command_item, std::ostream& out, uint32_t visibility_mask) const
{
  print_help(command_item.get_options(), &command_item, out, visibility_mask, false);
}

void
Printer::print_help(OptionGroup const& group, CommandItem const* current_command_item, std::ostream& out, uint32_t visibility_mask, bool skip_print_usage) const
{
  // FIXME: move this into PrettyPrinter
  const int terminal_width = std::min(get_terminal_width(), 120);
  const int column_min_width = 8;
  int column_width = column_min_width;

  { // Calculate left column width
    for (auto const& item : group.get_items())
    {
      if (auto* opt = dynamic_cast<Option*>(item.get())) {
        int width = 2; // add two leading space
        if (opt->get_short_name()) {
          width += 2; // "-a"
        }

        if (opt->get_long_name()) {
          width += static_cast<int>(opt->get_long_name()->size()) + 2; // "--foobar"
        }

        if (opt->get_short_name() && opt->get_long_name()) {
          width += 2; // ", "
        }

        if (opt->requires_argument()) {
          width += static_cast<int>(dynamic_cast<OptionWithArg&>(*opt).get_argument_name().size()) + 1; // "=ARG"
        }

        column_width = std::max(column_width, width);
      }
    }

    column_width = column_width+2; // add two trailing space
  }

  if (terminal_width < column_width * 3)
  {
    column_width -= (column_width*3 - terminal_width);
    column_width = std::max(column_width, max_column_width);
  }

  PrettyPrinter pprinter(terminal_width); // -1 so we have a whitespace on the right side

  pprinter.set_column_width(column_width);
  if (!skip_print_usage) {
    print_usage(current_command_item, out);
  }

  for (auto const& item : group.get_items()) {
    if (item->get_flags().get_visibility() & visibility_mask) {
      item->print(pprinter);
      if (auto const* rest_options_item = dynamic_cast<RestOptionsItem const*>(item.get())) {
        print_help(rest_options_item->get_options(), nullptr, out, visibility_mask, true);
      }
    }
  }
}

void
Printer::print_help(std::ostream& out, uint32_t visibility_mask) const
{
  print_help(m_options, nullptr, out, visibility_mask, false);
}

} // namespace argparser

/* EOF */
