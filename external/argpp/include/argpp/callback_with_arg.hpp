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

#ifndef HEADER_ARGPARSER_CALLBACK_WITH_ARG_HPP
#define HEADER_ARGPARSER_CALLBACK_WITH_ARG_HPP

#include "argument.hpp"

namespace argparser {

template<typename T>
class CallbackWithArg
{
public:
  CallbackWithArg(Argument<T> argument) :
    m_argument(std::move(argument)),
    m_callback()
  {}

  virtual ~CallbackWithArg() = default;

  void call(std::string_view text) const {
    if (m_callback) {
      m_callback(m_argument.convert(text));
    }
  }

  template<typename F>
  void then(F func) {
    assert(!m_callback);
    m_callback = func;
  }

  template<typename P>
  void store(P& place) {
    then([&place](T&& value){
      place = std::move(value);
    });
  }

  void append(T& place) {
    then([&place](T&& value) {
      place.emplace_back(std::move(value));
    });
  }

private:
  Argument<T> m_argument;
  std::function<void (T)> m_callback;
};

} // namespace argparser

#endif

/* EOF */
