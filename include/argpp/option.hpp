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

#ifndef HEADER_ARGPARSER_OPTION_HPP
#define HEADER_ARGPARSER_OPTION_HPP

#include <optional>
#include <string>
#include <string_view>

#include "callback.hpp"
#include "callback_with_arg.hpp"
#include "argument.hpp"
#include "item.hpp"

namespace argparser {

class Option : public Item
{
public:
  Option(std::optional<char> short_name, std::optional<std::string> long_name, std::string help, Flags const& flags) :
    Item(flags),
    m_short_name(short_name),
    m_long_name(std::move(long_name)),
    m_help(std::move(help))
  {}

  std::string const& get_help() const { return m_help; }

  std::optional<char> get_short_name() const { return m_short_name; }
  std::optional<std::string> const& get_long_name() const { return m_long_name; }

  void print(PrettyPrinter& printer) override;
  std::string text() const override;

  virtual bool requires_argument() const = 0;

private:
  std::optional<char> m_short_name;
  std::optional<std::string> m_long_name;
  std::string m_help;
};

class OptionWithoutArg : public Option,
                         public Callback
{
public:
  OptionWithoutArg(std::optional<char> short_name,
                   std::optional<std::string> long_name,
                   std::string help,
                   Flags const& flags) :
    Option(short_name, std::move(long_name), std::move(help), flags)
  {}

  bool requires_argument() const override { return false; }
};

class OptionWithArg : public Option
{
public:
  using Option::Option;
  virtual std::string const& get_argument_name() const = 0;
  virtual void call(std::string_view text) const = 0;
};

template<typename T>
class TOptionWithArg : public OptionWithArg,
                       public CallbackWithArg<T>
{
public:
  TOptionWithArg(std::optional<char> short_name,
                 std::optional<std::string> long_name, // NOLINT
                 Argument<T> argument,
                 std::string help, // NOLINT
                 Flags const& flags) :
    OptionWithArg(short_name, std::move(long_name), std::move(help), flags),
    CallbackWithArg<T>(argument),
    m_argument(std::move(argument))
  {}

  bool requires_argument() const override { return true; }

  std::string const& get_argument_name() const override {
    return m_argument.get_name();
  }

  void call(std::string_view text) const override {
    CallbackWithArg<T>::call(text);
  }

private:
  Argument<T> m_argument;
};

} // namespace argparser

#endif

/* EOF */
