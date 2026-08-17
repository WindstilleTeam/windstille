/*
**  Windstille - A Sci-Fi Action-Adventure Game
**  Copyright (C) 2021 Ingo Ruhnke <grumbel@gmail.com>
**
**  This program is free software: you can redistribute it and/or modify
**  it under the terms of the GNU General Public License as published by
**  the Free Software Foundation, either version 3 of the License, or
**  (at your option) any later version.
**
**  This program is distributed in the hope that it will be useful,
**  but WITHOUT ANY WARRANTY; without even the implied warranty of
**  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
**  GNU General Public License for more details.
**
**  You should have received a copy of the GNU General Public License
**  along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef HEADER_WSTGUI_RFLOAT_HPP
#define HEADER_WSTGUI_RFLOAT_HPP

namespace wstgui {

/** A float value that is either absolute or relative to some parent size */
class rfloat
{
private:
  rfloat(float value, bool absolute) :
    m_value(value),
    m_absolute(absolute)
  {}

public:
  static rfloat absolute(float value) { return rfloat(value, true); }
  static rfloat relative(float value) { return rfloat(value, false); }

  float resolve(float parent) const
  {
    if (m_absolute) {
      return m_value;
    } else {
      return parent * m_value;
    }
  }

  float operator()(float parent) const {
    return resolve(parent);
  }

  bool is_absolute() const { return m_absolute; }
  bool is_relative() const { return !m_absolute; }

  float raw() const { return m_value; }

private:
  float m_value;
  bool m_absolute;
};

} // namespace wstgui

#endif

/* EOF */
