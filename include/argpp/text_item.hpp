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

#ifndef HEADER_ARGPARSER_TEXT_ITEM_HPP
#define HEADER_ARGPARSER_TEXT_ITEM_HPP

#include <string>

#include "fwd.hpp"
#include "item.hpp"

namespace argparser {

class TextItem : public Item
{
public:
  TextItem(std::string text, Flags const& flags) :
    Item(flags),
    m_text(std::move(text))
  {}

  void print(PrettyPrinter& pprinter) override;

  std::string const& get_text() const { return m_text; }

private:
  std::string m_text;
};

class PseudoItem : public Item
{
public:
  PseudoItem(std::string name, std::string help, Flags const& flags) :
    Item(flags),
    m_name(std::move(name)),
    m_help(std::move(help))
  {}

  void print(PrettyPrinter& pprinter) override;

  std::string const& get_name() const { return m_name; }
  std::string const& get_help() const { return m_help; }

private:
  std::string m_name;
  std::string m_help;
};

} // namespace argparser

#endif

/* EOF */
