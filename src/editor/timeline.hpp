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

#ifndef HEADER_WINDSTILLE_EDITOR_TIMELINE_HPP
#define HEADER_WINDSTILLE_EDITOR_TIMELINE_HPP

#include "editor/timeline_handles.hpp"
#include "editor/timeline_properties.hpp"
#include "editor/object_model.hpp"
#include "math/vector2f.hpp"

class FileWriter;

class Timeline
{
public:
  typedef std::vector<TimelineLayerHandle> Layers;
  typedef Layers::iterator iterator;
  typedef Layers::const_iterator const_iterator;

private:
  Layers m_layers;

public:
  Timeline();

  iterator begin() { return m_layers.begin(); }
  iterator end()   { return m_layers.end();   }

  const_iterator begin() const { return m_layers.begin(); }
  const_iterator end()   const { return m_layers.end();   }

  int size() const { return static_cast<int>(m_layers.size()); }

  TimelineLayerHandle get_layer(int n) const;

  TimelineLayerHandle add_layer(const std::string& name);
  TimelineLayerHandle create_layer(const std::string& name);
  TimelineObjectLayerHandle create_object_layer(ObjectModelHandle object, TimelineProperty property);
  TimelineObjectLayerHandle get_object_layer(ObjectModelHandle object, TimelineProperty property);

  void add_layer(TimelineLayerHandle layer);
  void remove_layer(TimelineLayerHandle layer);

  void apply(float pos);

  void write(FileWriter& writer) const;

private:
  Timeline(const Timeline&);
  Timeline& operator=(const Timeline&);
};

#endif

/* EOF */
