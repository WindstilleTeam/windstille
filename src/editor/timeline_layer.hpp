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

#ifndef HEADER_WINDSTILLE_TIMELINE_LAYER_HPP
#define HEADER_WINDSTILLE_TIMELINE_LAYER_HPP

#include <vector>
#include <string>
#include <boost/shared_ptr.hpp>

#include "timeline_object.hpp"

class TimelineLayer
{
private:
  std::string m_name;
  std::vector<TimelineObjectHandle> m_objects;

public:
  TimelineLayer(const std::string& name);

  void add_object(TimelineObjectHandle object);

  TimelineObjectHandle get_object(float pos) const;

  void draw();

private:
  TimelineLayer(const TimelineLayer&);
  TimelineLayer& operator=(const TimelineLayer&);
};

typedef boost::shared_ptr<TimelineLayer> TimelineLayerHandle;

#endif

/* EOF */
