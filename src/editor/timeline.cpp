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

#include "editor/timeline.hpp"

#include "editor/timeline_object_layer.hpp"
#include "editor/timeline_layer.hpp"

Timeline::Timeline() :
  m_layers()
{
}

TimelineLayerHandle
Timeline::get_layer(int n) const
{
  if (!m_layers.empty() &&
      n >= 0 && n < static_cast<int>(m_layers.size()))
  {
    return m_layers[n];
  }
  else
  {
    return TimelineLayerHandle();
  }
}

TimelineLayerHandle
Timeline::create_layer(const std::string& name)
{
  return TimelineLayerHandle(new TimelineLayer(name));
}

TimelineLayerHandle
Timeline::add_layer(const std::string& name)
{
  m_layers.push_back(TimelineLayerHandle(new TimelineLayer(name)));

  return m_layers.back();
}

TimelineObjectLayerHandle
Timeline::create_object_layer(ObjectModelHandle object, TimelineProperty property)
{
  switch(property)
  {
    case kPosition:
      return TimelineObjectLayerHandle(new TimelineObjectDataLayer<Vector2f>(object, property));

    case kRotation:
      return TimelineObjectLayerHandle(new TimelineObjectDataLayer<float>(object, property));

    case kScale:
      return TimelineObjectLayerHandle(new TimelineObjectDataLayer<Vector2f>(object, property));

    default:
      throw std::runtime_error("Timeline::create_object_layer: unknown property given");
  }
}

void
Timeline::add_layer(TimelineLayerHandle layer)
{
  m_layers.push_back(layer);
}

void
Timeline::remove_layer(TimelineLayerHandle layer)
{
  m_layers.erase(std::remove(m_layers.begin(), m_layers.end(), layer), m_layers.end());
}

TimelineObjectLayerHandle
Timeline::get_object_layer(ObjectModelHandle object, TimelineProperty property)
{
  for(iterator i = begin(); i != end(); ++i)
  {
    TimelineObjectLayerHandle layer = boost::dynamic_pointer_cast<TimelineObjectLayer>(*i);

    if (layer &&
        layer->get_property() == property &&
        layer->get_object()   == object)
    {
      return layer;
    }
  }
  return TimelineObjectLayerHandle();
}

void
Timeline::apply(float pos)
{
  for(iterator i = begin(); i != end(); ++i)
  {
    (*i)->apply(pos);
  }
}

void
Timeline::write(FileWriter& writer) const
{
  writer.start_section("layers");
  for(const_iterator i = begin(); i != end(); ++i)
  {
    (*i)->write(writer);
  }
  writer.end_section();
}

/* EOF */
