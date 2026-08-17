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

#ifndef HEADER_ARGPARSER_ALIAS_ITEM_HPP
#define HEADER_ARGPARSER_ALIAS_ITEM_HPP

#include "item.hpp"

namespace argparser {

class LongOptionAlias : public Item
{
public:
  LongOptionAlias(std::string name, Option& option, Flags const& flags) :
    Item(flags),
    m_name(std::move(name)),
    m_option(option)
  {}

  std::string const& get_name() const { return m_name; }
  Option& get_option() const { return m_option; }

private:
  std::string m_name;
  Option& m_option;
};

class ShortOptionAlias : public Item
{
public:
  ShortOptionAlias(char name, Option& option, Flags const& flags) :
    Item(flags),
    m_name(name),
    m_option(option)
  {}

  char get_name() const { return m_name; }
  Option& get_option() const { return m_option; }

private:
  char m_name;
  Option& m_option;
};

} // namespace argparser

#endif

/* EOF */
