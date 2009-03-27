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

#ifndef HEADER_WINDSTILLE_EDITOR_HARD_LAYER_HPP
#define HEADER_WINDSTILLE_EDITOR_HARD_LAYER_HPP

#include <boost/shared_ptr.hpp>
#include <list>
#include <set>
#include <string>

#include "selection.hpp"
#include "object_model.hpp"

class HardLayer;
typedef boost::shared_ptr<HardLayer> HardLayerHandle;

class HardLayer
{
private:
  typedef std::list<ObjectModelHandle> Objects;

  Objects objects;

public:
  HardLayer();
  ~HardLayer();

  void add(const ObjectModelHandle& object);
  void remove(const ObjectModelHandle& object);

  void draw(SceneContext& sc, const Layers& layers);
  void update(float delta);

  ObjectModelHandle get_object_at(const Vector2f& pos, const Layers& layers) const;
  SelectionHandle   get_selection(const Rectf& rect, const Layers& layers) const;

  void raise_to_top(ObjectModelHandle object);
  void lower_to_bottom(ObjectModelHandle object);
  void raise(ObjectModelHandle object);
  void lower(ObjectModelHandle object);

  SnapData snap_object(const Rectf& rect, const std::set<ObjectModelHandle>& ignore_objects) const;

  void write(FileWriter& writer) const;

  HardLayerHandle clone() const;
  
private:
  HardLayer(const HardLayer&);
  HardLayer& operator=(const HardLayer&);
};

#endif

/* EOF */
