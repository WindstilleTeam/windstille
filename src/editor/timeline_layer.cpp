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

#include "editor/timeline_layer.hpp"

#include <assert.h>

#include "util/file_writer.hpp"
#include "editor/timeline_object.hpp"

TimelineLayer::TimelineLayer(const std::string& name) :
  m_name(name),
  m_objects()
{
}

void
TimelineLayer::add_object(TimelineObjectHandle object)
{
  m_objects.push_back(object);
}

void
TimelineLayer::remove_object(TimelineObjectHandle object)
{
  m_objects.erase(std::remove(m_objects.begin(), m_objects.end(), object), m_objects.end());
}

TimelineObjectHandle
TimelineLayer::get_object(float pos) const
{
  for(const_iterator i = begin(); i != end(); ++i)
  {
    if (pos >= (*i)->get_pos() &&
        pos <  (*i)->get_pos() + ((*i)->get_width() == 0 ? 1 : (*i)->get_width()))
    {
      return *i;
    }
  }

  return TimelineObjectHandle();
}

TimelineLayer::Objects
TimelineLayer::get_objects(float selection_start, float selection_end) const
{
  assert(selection_start <= selection_end);

  Objects objects;

  for(const_iterator i = begin(); i != end(); ++i)
  {
    if (selection_start <= (*i)->get_pos() &&
        selection_end   >  (*i)->get_pos() + (*i)->get_width())
    {
      objects.push_back(*i);
    }
  }

  return objects;
}

void
TimelineLayer::write(FileWriter& writer) const
{
  writer.start_section("layer");
  writer.start_section("objects");
  for(const_iterator i = begin(); i != end(); ++i)
  {
    (*i)->write(writer);
  }
  writer.end_section();
  writer.end_section();
}

/* EOF */
