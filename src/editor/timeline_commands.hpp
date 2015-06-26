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

#ifndef HEADER_WINDSTILLE_EDITOR_TIMELINE_COMMANDS_HPP
#define HEADER_WINDSTILLE_EDITOR_TIMELINE_COMMANDS_HPP

#include <boost/function.hpp>

#include "editor/layer.hpp"
#include "editor/object_model.hpp"
#include "editor/command.hpp"
#include "editor/timeline.hpp"
#include "editor/timeline_object_layer.hpp"
#include "editor/timeline_properties.hpp"

class TimelineAddKeyframeCommand : public Command
{
private:
  SectorModel&         m_sector;
  TimelineObjectLayerHandle m_layer;
  ObjectModelHandle    m_object;
  TimelineProperty     m_property;
  TimelineObjectHandle m_keyframe;
  bool m_add_layer;

public:
  TimelineAddKeyframeCommand(SectorModel& sector, ObjectModelHandle object, TimelineProperty property, float pos) :
    m_sector(sector),
    m_layer(),
    m_object(object),
    m_property(property),
    m_keyframe(),
    m_add_layer(false)
  {
    m_layer = m_sector.get_timeline()->get_object_layer(m_object, m_property);
    if (!m_layer)
    {
      m_layer = m_sector.get_timeline()->create_object_layer(m_object, m_property);
      m_add_layer = true;
    }

    m_keyframe = m_layer->create_keyframe(pos);
  }

  void redo()
  {
    if (m_add_layer)
      m_sector.get_timeline()->add_layer(m_layer);

    m_layer->add_object(m_keyframe);
  }

  void undo()
  {
    if (m_add_layer)
      m_layer->remove_object(m_keyframe);

    if (m_layer->empty())
    {
      m_sector.get_timeline()->remove_layer(m_layer);
    }
  }
};

class TimelineAddLayerCommand : public Command
{
private:
  SectorModel&        m_sector;
  TimelineLayerHandle m_layer;

public:
  TimelineAddLayerCommand(SectorModel& sector, const std::string& name) :
    m_sector(sector),
    m_layer()
  {
    m_layer = m_sector.get_timeline()->create_layer(name);
  }

  void redo()
  {
    m_sector.get_timeline()->add_layer(m_layer);
  }

  void undo()
  {
    m_sector.get_timeline()->remove_layer(m_layer);
  }
};

#endif

/* EOF */
