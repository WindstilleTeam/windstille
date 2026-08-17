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

#ifndef HEADER_ARGPARSER_CALLBACK_HPP
#define HEADER_ARGPARSER_CALLBACK_HPP

#include <functional>

namespace argparser {

class Callback
{
public:
  Callback() :
    m_callback()
  {
  }

  virtual ~Callback() = default;

  void call() const {
    if (m_callback) {
      m_callback();
    }
  }

  void then(std::function<void ()> cb) {
    m_callback = std::move(cb);
  }

  template<typename T>
  void store(T& place, T const value) {
    then([&place, value = std::move(value)]{
      place = value;
    });
  }

  template<typename T>
  void increment(T& place) {
    then([&place]() {
      ++place;
    });
  }

  template<typename T>
  void append(T& place, T const value) {
    then([&place, value = std::move(value)]{
      place.emplace_back(std::move(value));
    });
  }

private:
  std::function<void ()> m_callback;
};

} // namespace argparser

#endif

/* EOF */
