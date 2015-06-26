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

#ifndef HEADER_WINDSTILLE_EDITOR_TIMELINE_LAYER_HPP
#define HEADER_WINDSTILLE_EDITOR_TIMELINE_LAYER_HPP

#include <vector>
#include <string>
#include <boost/shared_ptr.hpp>

#include "timeline_handles.hpp"

class FileWriter;

class TimelineLayer
{
public:
  typedef std::vector<TimelineObjectHandle> Objects;
  typedef std::vector<TimelineObjectHandle>::iterator iterator;
  typedef std::vector<TimelineObjectHandle>::const_iterator const_iterator;

private:
  std::string m_name;
  Objects m_objects;

public:
  TimelineLayer(const std::string& name);
  virtual ~TimelineLayer() {}

  iterator begin() { return m_objects.begin(); }
  iterator end()   { return m_objects.end();   }

  const_iterator begin() const { return m_objects.begin(); }
  const_iterator end()   const { return m_objects.end();   }

  int size() const { return static_cast<int>(m_objects.size()); }

  bool empty() const { return m_objects.empty(); }

  void add_object(TimelineObjectHandle object);
  void remove_object(TimelineObjectHandle object);

  Objects get_objects(float start, float end) const;
  TimelineObjectHandle get_object(float pos) const;

  std::string get_name() const { return m_name; }
  virtual void apply(float pos) {}

  virtual void write(FileWriter& writer) const;

private:
  TimelineLayer(const TimelineLayer&);
  TimelineLayer& operator=(const TimelineLayer&);
};

#endif

/* EOF */
