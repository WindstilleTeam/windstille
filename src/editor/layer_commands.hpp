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

#ifndef HEADER_WINDSTILLE_EDITOR_LAYER_COMMANDS_HPP
#define HEADER_WINDSTILLE_EDITOR_LAYER_COMMANDS_HPP

#include "editor/layer.hpp"
#include "editor/command.hpp"
#include "editor/sector_model.hpp"

class LayerAddCommand : public Command
{
private:
  SectorModel& sector;
  Gtk::TreeModel::Path path;

public:
  LayerAddCommand(SectorModel& sector_, const Gtk::TreeModel::Path& path_)
    : sector(sector_),
      path(path_)
  {}

  void redo() {
    // FIXME: We should recycle the actual layer object instead of creating a new one
    sector.add_layer("New Layer", path);
  }

  void undo() {
    sector.delete_layer(path);
  }

private:
  LayerAddCommand(const LayerAddCommand&);
  LayerAddCommand& operator=(const LayerAddCommand&);
};

class LayerDeleteCommand : public Command
{
private:
  SectorModel& sector;
  Gtk::TreeModel::Path path;
  LayerHandle layer;

public:
  LayerDeleteCommand(SectorModel& sector_, const Gtk::TreeModel::Path& path_)
    : sector(sector_),
      path(path_),
      layer(sector.get_layer(path))
  {}

  void redo() {
    sector.delete_layer(path);
  }

  void undo() {
    sector.add_layer(layer/*, path*/);
  }

private:
  LayerDeleteCommand(const LayerDeleteCommand&);
  LayerDeleteCommand& operator=(const LayerDeleteCommand&);
};

#endif

/* EOF */
