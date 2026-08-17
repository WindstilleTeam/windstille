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

#ifndef HEADER_ARGPARSER_REST_OPTIONS_ITEM_HPP
#define HEADER_ARGPARSER_REST_OPTIONS_ITEM_HPP

#include "callback_with_arg.hpp"
#include "fwd.hpp"
#include "item.hpp"
#include "option_group.hpp"

namespace argparser {

class RestOptionsItem : public Item
{
public:
  RestOptionsItem(Flags const& flags) :
    Item(flags)
  {}

  void print(PrettyPrinter& pprinter) override;

  std::string text() const override { return get_name(); }

  virtual std::string const& get_name() const = 0;
  virtual std::string const& get_help() const = 0;
  virtual void call(std::string_view text) const = 0;
  virtual OptionGroup& get_options() = 0;
  virtual OptionGroup const& get_options() const = 0;
};

template<typename T>
class TRestOptionsItem : public RestOptionsItem,
                         public CallbackWithArg<T>
{
public:
  TRestOptionsItem(Argument<T> argument, std::string help, Flags const& flags) :
    RestOptionsItem(flags),
    CallbackWithArg<T>(argument),
    m_argument(argument),
    m_help(std::move(help)),
    m_options()
  {}

  std::string const& get_name() const override {
    return m_argument.get_name();
  }

  std::string const& get_help() const override {
    return m_help;
  }

  void call(std::string_view text) const override {
    CallbackWithArg<T>::call(text);
  }

  OptionGroup& get_options() override { return m_options; }
  OptionGroup const& get_options() const override { return m_options; }

private:
  Argument<T> m_argument;
  std::string m_help;
  OptionGroup m_options;
};

} //namespace argparser

#endif

/* EOF */
