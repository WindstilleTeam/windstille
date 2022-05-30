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

#ifndef HEADER_WINDSTILLE_EDITOR_OBJECT_COMMANDS_HPP
#define HEADER_WINDSTILLE_EDITOR_OBJECT_COMMANDS_HPP

#include "editor/layer.hpp"
#include "editor/object_model.hpp"
#include "editor/command.hpp"

namespace windstille {


class ObjectRemoveCommand : public Command
{
private:
  LayerHandle          layer;
  ObjectModelHandle    object;

public:
  ObjectRemoveCommand(SectorModel& sector, ObjectModelHandle object_)
    : layer(sector.get_layer(object_)),
      object(object_)
  {}

  void redo() override {
    if (layer)
      layer->remove(object);
    else
      std::cout << "ObjectRemoveCommand: null layer" << std::endl;
  }

  void undo() override {
    if (layer)
      layer->add(object);
  }

};


class ObjectAddCommand : public Command
{
private:
  LayerHandle          layer;
  ObjectModelHandle    object;

public:
  ObjectAddCommand(LayerHandle layer_, ObjectModelHandle object_)
    : layer(layer_),
      object(object_)
  {}

  void redo() override {
    if (layer)
      layer->add(object);
    else
      std::cout << "ObjectRemoveCommand: null layer" << std::endl;
  }

  void undo() override {
    if (layer)
      layer->remove(object);
  }
};


class ObjectSetPosCommand : public Command
{
private:
  ObjectModelHandle    object;
  glm::vec2             orig_pos;
  glm::vec2             new_pos;

public:
  ObjectSetPosCommand(ObjectModelHandle object_, glm::vec2 const& new_pos_)
    : object(object_),
      orig_pos(object->get_rel_pos()),
      new_pos(new_pos_)
  {}

  void redo() override {
    object->set_rel_pos(new_pos);
  }

  void undo() override {
    object->set_rel_pos(orig_pos);
  }
};


} // namespace windstille

#endif

/* EOF */
