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

#ifndef HEADER_WINDSTILLE_EDITOR_TIMELINE_KEYFRAME_OBJECT_HPP
#define HEADER_WINDSTILLE_EDITOR_TIMELINE_KEYFRAME_OBJECT_HPP

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

  void  set_pos(float pos) override { m_pos = pos; }
  float get_pos()   const override { return m_pos; }
  float get_width() const override { return 0.0f; }

private:
  TimelineKeyframeObject(TimelineKeyframeObject const&);
  TimelineKeyframeObject& operator=(TimelineKeyframeObject const&);
};

template<typename C>
class TimelineKeyframeDataObject : public TimelineKeyframeObject
{
private:
  C m_data;

public:
  TimelineKeyframeDataObject(float pos, C const& data)
    : TimelineKeyframeObject(pos),
      m_data(data)
  {}

  C const& get_data() const { return m_data; }
  void     set_data(C const& data) { m_data = data; }

  void write(FileWriter& writer) const override
  {
    writer.begin_object("keyframe");
    writer.write("pos",  m_pos);
    writer.write("data", m_data);
    writer.end_object();
  }

  C blend(TimelineKeyframeDataObject<C> const& rhs, float pos)
  {
    float rel_pos = pos - get_pos();
    float range   = rhs.get_pos() - m_pos;
    float alpha   = rel_pos / range;

    return ((1.0f - alpha) * get_data()) + (alpha * rhs.get_data());
  }

private:
  TimelineKeyframeDataObject(TimelineKeyframeDataObject const&);
  TimelineKeyframeDataObject& operator=(TimelineKeyframeDataObject const&);
};

#endif

/* EOF */
