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

#ifndef HEADER_WINDSTILLE_TIMELINE_OBJECT_LAYER_HPP
#define HEADER_WINDSTILLE_TIMELINE_OBJECT_LAYER_HPP

#include "editor/timeline_layer.hpp"
#include "editor/object_model.hpp"

class TimelineObjectLayer : public TimelineLayer
{
private:
  ObjectModelHandle m_object;
  TimelineProperty  m_property;

public:
  TimelineObjectLayer(ObjectModelHandle object, TimelineProperty property);

private:
  TimelineObjectLayer(const TimelineObjectLayer&);
  TimelineObjectLayer& operator=(const TimelineObjectLayer&);
};

template<typename C>
class TimelineObjectDataLayer : public TimelineObjectLayer
{
public:
  TimelineObjectDataLayer(ObjectModelHandle object, TimelineProperty property)
    : TimelineObjectLayer(object, property)
  {}

  void apply(float pos)
  {
    //object->set_property(property, value);
  }
};

#endif

/* EOF */
