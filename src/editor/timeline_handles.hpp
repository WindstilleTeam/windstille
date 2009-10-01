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

#ifndef HEADER_WINDSTILLE_EDITOR_TIMELINE_HANDLES_HPP
#define HEADER_WINDSTILLE_EDITOR_TIMELINE_HANDLES_HPP

#include <boost/shared_ptr.hpp>

class Timeline;
class TimelineLayer;
class TimelineObject;
class TimelineObjectLayer;

typedef boost::shared_ptr<Timeline>            TimelineHandle;
typedef boost::shared_ptr<TimelineLayer>       TimelineLayerHandle;
typedef boost::shared_ptr<TimelineObject>      TimelineObjectHandle;
typedef boost::shared_ptr<TimelineObjectLayer> TimelineObjectLayerHandle;

#endif

/* EOF */
