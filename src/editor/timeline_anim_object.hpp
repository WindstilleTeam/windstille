/*
**  Windstille - A Sci-Fi Action-Adventure Game
**  Copyright (C) 2009 Ingo Ruhnke <grumbel@gmx.de>
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

#ifndef HEADER_WINDSTILLE_TIMELINE_ANIM_OBJECT_HPP
#define HEADER_WINDSTILLE_TIMELINE_ANIM_OBJECT_HPP

#include <string>

#include "editor/timeline_object.hpp"

class TimelineAnimObject : public TimelineObject
{
private:
  float m_pos;
  float m_width;
  std::string m_name;

public:
  TimelineAnimObject(float pos, float width, const std::string& name) :
    m_pos(pos),
    m_width(width),
    m_name(name)
  {}

  float get_pos()   const { return m_pos; }
  void  set_pos(float pos) { m_pos = pos; }
  float get_width() const { return m_width; }
  std::string get_name() const { return m_name; }

private:
  TimelineAnimObject(const TimelineAnimObject&);
  TimelineAnimObject& operator=(const TimelineAnimObject&);
};

#endif

/* EOF */
