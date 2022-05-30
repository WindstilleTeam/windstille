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

#ifndef HEADER_WINDSTILLE_EDITOR_TIMELINE_OBJECT_LAYER_HPP
#define HEADER_WINDSTILLE_EDITOR_TIMELINE_OBJECT_LAYER_HPP

#include <memory>
#include <iostream>

#include "editor/timeline_layer.hpp"
#include "editor/timeline_keyframe_object.hpp"
#include "editor/object_model.hpp"

namespace windstille {

class TimelineObjectLayer : public TimelineLayer
{
protected:
  ObjectModelHandle m_object;
  TimelineProperty  m_property;

public:
  TimelineObjectLayer(ObjectModelHandle object, TimelineProperty property);

  virtual TimelineObjectHandle create_keyframe(float pos) =0;

  TimelineProperty  get_property() const { return m_property; }
  ObjectModelHandle get_object()   const { return m_object; }

  void write(FileWriter& writer) const override
  {
    writer.begin_object("object-layer");
    writer.write("object",   m_object->get_id());
    writer.write("property", static_cast<int>(m_property)); // FIXME: convert property to string
    writer.begin_collection("objects");
    for(const_iterator i = begin(); i != end(); ++i)
    {
      (*i)->write(writer);
    }
    writer.end_collection();
    writer.end_object();
  }

private:
  TimelineObjectLayer(TimelineObjectLayer const&);
  TimelineObjectLayer& operator=(TimelineObjectLayer const&);
};


template<typename C>
class TimelineObjectDataLayer : public TimelineObjectLayer
{
public:
  TimelineObjectDataLayer(ObjectModelHandle object, TimelineProperty property)
    : TimelineObjectLayer(object, property)
  {}

  C get_value(float pos)
  {
    if (empty())
    {
      return C();
    }
    else if (size() == 1)
    {
      std::shared_ptr<TimelineKeyframeDataObject<C> > keyframe =
        std::dynamic_pointer_cast<TimelineKeyframeDataObject<C> >(*begin());
      return keyframe->get_data();
    }
    else
    {
      std::shared_ptr<TimelineKeyframeDataObject<C> > lhs;
      std::shared_ptr<TimelineKeyframeDataObject<C> > rhs;

      for(iterator i = begin(); i != end(); ++i)
      {
        //std::cout << size() << " - " << (i - begin()) << std::endl;

        std::shared_ptr<TimelineKeyframeDataObject<C> > keyframe =
          std::dynamic_pointer_cast<TimelineKeyframeDataObject<C> >(*i);

        assert(keyframe);

        //std::cout << "keyframe: " << keyframe.get() << std::endl;

        if (!lhs && keyframe->get_pos() <= pos)
        {
          //std::cout << "crash1" << std::endl;
          lhs = keyframe;
        }
        else if (lhs)
        {
          //std::cout << "lhs: " << lhs->get_pos() << " " << keyframe->get_pos() << std::endl;

          if (keyframe->get_pos() >= lhs->get_pos() &&
              keyframe->get_pos() <= pos)
          {
            lhs = keyframe;
          }
        }

        //std::cout << "snip: " << lhs.get() << " " << rhs.get() << " " << keyframe.get() << std::endl;

        if (!rhs && keyframe->get_pos() >= pos)
        {
          //std::cout << "crash2" << std::endl;
          rhs = keyframe;
        }
        else if (rhs)
        {
          //std::cout << "rhs: " << rhs->get_pos() << " " << keyframe->get_pos() << std::endl;

          if (keyframe->get_pos() <= rhs->get_pos() &&
              keyframe->get_pos() >= pos)
          {
            rhs = keyframe;
          }
        }

        //std::cout << "Loop exit: " << lhs.get() << " " << rhs.get() << std::endl;
      }

      //std::cout << "PTR: " << lhs.get() << " " << rhs.get() << std::endl;

      if (rhs && lhs)
      {
        if (lhs == rhs)
        {
          return lhs->get_data();
        }
        else
        {
          //std::cout << "POS: " << pos << " " << lhs->get_pos() << " " << rhs->get_pos() << std::endl;
          //std::cout << size() << " " << lhs->get_data() << " " << rhs->get_data() << std::endl;
          return lhs->blend(*rhs, pos);
        }
      }
      else if (rhs)
      {
        return rhs->get_data();
      }
      else if (lhs)
      {
        return lhs->get_data();
      }
      else
      {
        std::cout << "Error: TimelineObjectDataLayer" << std::endl;
        return C();
      }
    }
  }

  void apply(float pos) override
  {
    //std::cout << "apply: " << pos << " " << get_value(pos) << std::endl;
    m_object->set_property(m_property, get_value(pos));
  }

  TimelineObjectHandle create_keyframe(float pos) override
  {
    C data;
    m_object->get_property(m_property, data);
    return TimelineObjectHandle(new TimelineKeyframeDataObject<C>(pos, data));
  }
};

typedef std::shared_ptr<TimelineObjectLayer> TimelineObjectLayerHandle;

} // namespace windstille

#endif

/* EOF */
