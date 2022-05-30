/*
**  Windstille - A Sci-Fi Action-Adventure Game
**  Copyright (C) 2009 Ingo Ruhnke <grumbel@gmail.com>
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

#ifndef HEADER_WINDSTILLE_EDITOR_TIMELINE_SOUND_OBJECT_HPP
#define HEADER_WINDSTILLE_EDITOR_TIMELINE_SOUND_OBJECT_HPP

#include "editor/timeline_object.hpp"

class TimelineSoundObject : public TimelineObject
{
private:
  float m_pos;
  float m_width;
  std::string m_name;

public:
  TimelineSoundObject(float pos, float width, std::string const& name) :
    m_pos(pos),
    m_width(width),
    m_name(name)
  {}

  float get_pos()   const override { return m_pos; }
  void  set_pos(float pos) override { m_pos = pos; }
  float get_width() const override { return m_width; }
  std::string get_name() const { return m_name; }

  void write(FileWriter& writer) const override
  {
    writer.begin_object("sound");
    writer.write("pos",  m_pos);
    writer.write("name", m_name);
    writer.end_object();
  }

private:
  TimelineSoundObject(TimelineSoundObject const&);
  TimelineSoundObject& operator=(TimelineSoundObject const&);
};

#endif

/* EOF */
