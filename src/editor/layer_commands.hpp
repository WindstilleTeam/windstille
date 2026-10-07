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

#ifndef HEADER_WINDSTILLE_EDITOR_LAYER_COMMANDS_HPP
#define HEADER_WINDSTILLE_EDITOR_LAYER_COMMANDS_HPP

#include <cstddef>
#include "editor/command.hpp"
#include "editor/layer.hpp"
#include "editor/sector_model.hpp"

namespace windstille {

class LayerAddCommand : public Command
{
private:
  SectorModel& sector;
  std::size_t insert_index;
  LayerHandle layer;
public:
  LayerAddCommand(SectorModel& sector_, std::size_t insert_index_ = SectorModel::npos)
    : sector(sector_), insert_index(insert_index_), layer() {}
  void redo() override {
    if (!layer) layer = sector.add_layer("New Layer", insert_index);
    else sector.add_layer(layer, insert_index);
  }
  void undo() override { if (layer) sector.delete_layer(layer); }
private:
  LayerAddCommand(LayerAddCommand const&) = delete;
  LayerAddCommand& operator=(LayerAddCommand const&) = delete;
};

class LayerDeleteCommand : public Command
{
private:
  SectorModel& sector;
  LayerHandle layer;
  std::size_t index;
public:
  LayerDeleteCommand(SectorModel& sector_, LayerHandle layer_)
    : sector(sector_), layer(layer_), index(sector_.get_layer_index(layer_)) {}
  void redo() override { sector.delete_layer(layer); }
  void undo() override { sector.add_layer(layer, index); }
private:
  LayerDeleteCommand(LayerDeleteCommand const&) = delete;
  LayerDeleteCommand& operator=(LayerDeleteCommand const&) = delete;
};

} // namespace windstille
#endif
/* EOF */
