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

#include <fmt/format.h>

#include <map>
#include <span>
#include <string_view>

#include "command_item.hpp"
#include "error.hpp"
#include "fwd.hpp"
#include "option_group.hpp"
#include "parser.hpp"
#include "positional_item.hpp"
#include "rest_options_item.hpp"
#include "text_item.hpp"

namespace argparser {

class ParseContext
{
public:
  ParseContext(std::span<char const* const> argv) :
    m_idx(0),
    m_argv(argv),
    m_positional_index(0),
    m_mutual_exclusion_state(0u),
    m_parsed_arguments()
  {}

  std::string_view arg() const {
    return m_argv[m_idx];
  }

  bool next() {
    if (m_idx < m_argv.size() - 1) {
      m_idx += 1;
      return true;
    } else {
      return false;
    }
  }

  void rollback() {
    assert(m_idx > 0);
    m_idx -= 1;
  }

  std::string_view program() const { return m_argv[0]; }

  void incr_positional_index() { m_positional_index += 1; }
  int get_positional_index() const { return m_positional_index; }

  void add_mutual_exclusion_state(uint32_t mask) { m_mutual_exclusion_state |= mask; }
  uint32_t get_mutual_exclusion_state() const { return m_mutual_exclusion_state; }

  void add_parsed_argument(std::string_view arg, Item const& item) {
    m_parsed_arguments[&item] = arg;
  }

  std::map<Item const*, std::string_view> const& get_parsed_arguments() const { return m_parsed_arguments; }

private:
  size_t m_idx;
  std::span<char const* const> m_argv;
  int m_positional_index;
  uint32_t m_mutual_exclusion_state;
  std::map<Item const*, std::string_view> m_parsed_arguments;
};

Parser::Parser(OptionGroup const& options) :
  m_options(options)
{
}

void
Parser::parse_args(std::span<char const* const> argv)
{
  ParseContext ctx(argv);
  parse_args(ctx, m_options);
}

void
Parser::parse_args(ParseContext& ctx, OptionGroup const& group, bool options_only)
{
  while (ctx.next())
  {
    std::string_view const arg = ctx.arg();

    if (arg.empty() || arg[0] != '-') // rest
    {
      if (options_only) { ctx.rollback(); return; }
      parse_non_option(ctx, group, arg);
    }
    else if (arg[1] == '-') // --...
    {
      if (arg.size() == 2) { // --
        if (options_only) { ctx.rollback(); return; }

        // Got a '--' treat everything after this as rest
        while (ctx.next()) {
          parse_non_option(ctx, group, arg);
        }
        break;
      } else {
        parse_long_option(ctx, group, arg);
      }
    }
    else // short option
    {
      if (arg.size() == 1) { // -
        if (options_only) { ctx.rollback(); return; }
        parse_non_option(ctx, group, arg);
      } else {
        parse_short_option(ctx, group, arg);
      }
    }
  }

  // check required arguments
  for (auto const& item: group.get_items()) {
    if (item->get_flags().is_required()) {
      bool found = false;
      for(auto const& parsed_arg : ctx.get_parsed_arguments()) {
        if (parsed_arg.first == item.get()) {
          found = true;
          break;
        }
      }
      if (!found) {
        throw Error(fmt::format("required argument '{}' not found", item->text()));
      }
    }
  }
}

void
Parser::parse_non_option(ParseContext& ctx, OptionGroup const& group, std::string_view arg)
{
  if (group.has_commands()) {
    CommandItem const& command_item = group.lookup_command(arg);
    verify(ctx, arg, command_item);
    parse_args(ctx, command_item.get_options());
  } else {
    if (ctx.get_positional_index() < group.get_positional_count() ) {
      PositionalItem const& item = group.lookup_positional(ctx.get_positional_index());
      verify(ctx, arg, item);
      try {
        item.call(arg);
      } catch (std::exception const& err) {
        throw Error(fmt::format("invalid positional item at {}: {}: {}", ctx.get_positional_index(), arg, err.what()));
      } catch (...) {
        std::throw_with_nested(Error(fmt::format("invalid positional item at position {}: {}", ctx.get_positional_index(), arg)));
      }
      ctx.incr_positional_index();
    } else if (group.has_rest()) {
      RestItem const& item = group.lookup_rest();
      verify(ctx, arg, item);
      try {
        item.call(arg);
      } catch (std::exception const& err) {
        throw Error(fmt::format("invalid rest item at {}: {}: {}", ctx.get_positional_index(), arg, err.what()));
      } catch (...) {
        std::throw_with_nested(Error(fmt::format("invalid rest item at {}: {}", ctx.get_positional_index(), arg)));
      }
    } else if (group.has_rest_options()) {
      RestOptionsItem const& item = group.lookup_rest_options();
      verify(ctx, arg, item);
      try {
        item.call(arg);
      } catch (std::exception const& err) {
        throw Error(fmt::format("invalid rest item at {}: {}: {}", ctx.get_positional_index(), arg, err.what()));
      } catch (...) {
        std::throw_with_nested(Error(fmt::format("invalid rest item at {}: {}", ctx.get_positional_index(), arg)));
      }
      parse_args(ctx, item.get_options(), true);
    } else {
      throw Error(fmt::format("unknown item in position {}: {}", ctx.get_positional_index(), arg));
    }
  }
}

void
Parser::parse_long_option(ParseContext& ctx, OptionGroup const& group, std::string_view arg)
{
  std::string_view opt = arg.substr(2);
  std::string::size_type const equal_pos = opt.find('=');
  if (equal_pos != std::string::npos) // --long-opt=with-arg
  {
    std::string_view opt_arg = opt.substr(equal_pos + 1);
    opt = opt.substr(0, equal_pos);

    Option const& option = group.lookup_long_option(opt);
    verify(ctx, arg, option);
    if (option.requires_argument()) {
      dynamic_cast<OptionWithArg const&>(option).call(opt_arg);
    } else {
      throw Error("error doesn't need arg");
    }
  }
  else
  {
    Option const& option = group.lookup_long_option(opt);
    verify(ctx, arg, option);
    if (!option.requires_argument()) {
      dynamic_cast<OptionWithoutArg const&>(option).call();
    } else {
      if (!ctx.next()) {
        throw Error("option '" + std::string(arg) + "' requires an argument");
      }
      dynamic_cast<OptionWithArg const&>(option).call(ctx.arg());
    }
  }
}

void
Parser::parse_short_option(ParseContext& ctx, OptionGroup const& group, std::string_view arg)
{
  std::string_view const opts = arg.substr(1);

  for (size_t opts_i = 0; opts_i < opts.size(); ++opts_i) {
    char const opt = opts[opts_i];
    Option const& option = group.lookup_short_option(opt);
    verify(ctx, arg, option);
    if (!option.requires_argument()) {
      dynamic_cast<OptionWithoutArg const&>(option).call();
    } else {
      if (opts_i != opts.size() - 1) { // -fARG
        dynamic_cast<OptionWithArg const&>(option).call(opts.substr(opts_i + 1));
        break;
      } else { // -f ARG
        if (!ctx.next()) {
          throw Error("option '" + std::string(arg) + "' requires an argument");
        }
        dynamic_cast<OptionWithArg const&>(option).call(ctx.arg());
      }
    }
  }
}

void
Parser::verify(ParseContext& ctx, std::string_view arg, Item const& item)
{
  check_mutual_exclusion(ctx, arg, item);
  ctx.add_parsed_argument(arg, item);
}

void
Parser::check_mutual_exclusion(ParseContext& ctx, std::string_view arg, Item const& item)
{
  if (ctx.get_mutual_exclusion_state() & item.get_flags().get_mutual_exclusion()) {
    // find the option that caused the conflict
    std::string_view conflict_opt = "<unknown>";
    for(auto const& parsed_arg : ctx.get_parsed_arguments()) {
      if (parsed_arg.first->get_flags().get_mutual_exclusion() & item.get_flags().get_mutual_exclusion()) {
        conflict_opt = parsed_arg.second;
        break;
      }
    }

    throw Error(fmt::format("option '{}' conflicts with '{}'", arg, conflict_opt));
  } else {
    ctx.add_mutual_exclusion_state(item.get_flags().get_mutual_exclusion());
  }
}

} // namespace argparser

/* EOF */
