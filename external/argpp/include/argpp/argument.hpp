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

#ifndef HEADER_ARGPARSER_ARGUMENT_HPP
#define HEADER_ARGPARSER_ARGUMENT_HPP

#include <functional>
#include <string_view>

#include "from_string.hpp"

namespace argparser {

class ArgumentBase
{
public:
  ArgumentBase(std::string name) :
    m_name(std::move(name))
  {}
  virtual ~ArgumentBase() = default;

  std::string const& get_name() const { return m_name; }

private:
  std::string m_name;
};

template<typename T = std::string_view>
class Argument : public ArgumentBase
{
public:
  Argument(std::string name) : // NOLINT
    ArgumentBase(std::move(name)),
    m_convert_func(from_string<T>)
  {}

  template<typename F>
  Argument(std::string name, F func) : // NOLINT
    ArgumentBase(std::move(name)),
    m_convert_func(func)
  {}

  T convert(std::string_view text) const {
    return m_convert_func(text);
  }

private:
  std::function<T (std::string_view)> m_convert_func;
};

// Class template argument deduction (CTAD)
template<typename F>
Argument(std::string name, F func) -> Argument<typename std::result_of<F()>::type>;

} // namespace argparser

#endif

/* EOF */
