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

#ifndef HEADER_WINDSTILLE_TIMELINE_KEYFRAME_OBJECT_HPP
#define HEADER_WINDSTILLE_TIMELINE_KEYFRAME_OBJECT_HPP

#include "editor/timeline_object.hpp"

#include "util/file_writer.hpp"

class TimelineKeyframeObject : public TimelineObject
{
protected:
  float m_pos; 

public:
  TimelineKeyframeObject(float pos) :
    m_pos(pos)
  {}

  void  set_pos(float pos) { m_pos = pos; }
  float get_pos()   const { return m_pos; }
  float get_width() const { return 0.0f; }

private:
  TimelineKeyframeObject(const TimelineKeyframeObject&);
  TimelineKeyframeObject& operator=(const TimelineKeyframeObject&);
};

template<typename C>
class TimelineKeyframeDataObject : public TimelineKeyframeObject
{
private:
  C m_data;

public:
  TimelineKeyframeDataObject(float pos, const C& data)
    : TimelineKeyframeObject(pos),
      m_data(data)
  {}

  const C& get_data() const { return m_data; }
  void     set_data(const C& data) { m_data = data; }

  void write(FileWriter& writer) const 
  {
    writer.start_section("keyframe");
    writer.write("pos",  m_pos);
    writer.write("data", m_data);
    writer.end_section();
  }

  C blend(const TimelineKeyframeDataObject<C>& rhs, float pos)
  {
    float rel_pos = pos - get_pos();
    float range   = rhs.get_pos() - m_pos;
    float alpha   = rel_pos / range;

    return ((1.0f - alpha) * get_data()) + (alpha * rhs.get_data());
  }

private:
  TimelineKeyframeDataObject(const TimelineKeyframeDataObject&);
  TimelineKeyframeDataObject& operator=(const TimelineKeyframeDataObject&);
};

#endif

/* EOF */
