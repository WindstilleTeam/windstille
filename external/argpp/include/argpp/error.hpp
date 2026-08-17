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

#ifndef HEADER_ARGPARSER_ERROR_HPP
#define HEADER_ARGPARSER_ERROR_HPP

#include <stdexcept>

namespace argparser {

class Error : public std::exception
{
public:
  Error(std::string message) :
    m_message(std::move(message))
  {}

  const char* what() const noexcept override {
    return m_message.c_str();
  }

private:
  std::string const m_message;
};

} // namespace argparser

#endif

/* EOF */
