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

#ifndef HEADER_WINDSTILLE_EDITOR_TIMELINE_OBJECT_HPP
#define HEADER_WINDSTILLE_EDITOR_TIMELINE_OBJECT_HPP

#include <memory>

class TimelineObject
{
private:
public:
  TimelineObject() {}
  virtual ~TimelineObject() {}

  virtual float get_pos() const =0;
  virtual float get_width() const =0;
  virtual void  set_pos(float pos) =0;

  virtual void write(FileWriter& writer) const =0;

private:
  TimelineObject(TimelineObject const&);
  TimelineObject& operator=(TimelineObject const&);
};

#endif

/* EOF */
