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

#ifndef HEADER_WINDSTILLE_EDITOR_NAVGRAPH_COMMANDS_HPP
#define HEADER_WINDSTILLE_EDITOR_NAVGRAPH_COMMANDS_HPP

#include <iostream>

#include "editor/command.hpp"
#include "editor/navigation_graph_model.hpp"

namespace windstille {

class NavGraphNodeAddCommand : public Command
{
private:
  SectorModel& m_sector;
  std::shared_ptr<NavGraphNodeObjectModel> m_node;

public:
  NavGraphNodeAddCommand(SectorModel& sector,
                         std::shared_ptr<NavGraphNodeObjectModel> node)
    : m_sector(sector),
      m_node(node)
  {}

  void redo() override
  {
    m_sector.get_nav_graph().add_node(m_node);
  }

  void undo() override
  {
    m_sector.get_nav_graph().remove_node(m_node);
  }
};

class NavGraphNodeRemoveCommand : public Command
{
private:
  SectorModel& m_sector;
  std::shared_ptr<NavGraphNodeObjectModel> m_node;

public:
  NavGraphNodeRemoveCommand(SectorModel& sector,
                            std::shared_ptr<NavGraphNodeObjectModel> node)
    : m_sector(sector),
      m_node(node)
  {}

  void redo() override
  {
    m_sector.get_nav_graph().remove_node(m_node);
  }

  void undo() override
  {
    m_sector.get_nav_graph().add_node(m_node);
  }
};

class NavGraphEdgeAddCommand : public Command
{
private:
  SectorModel& m_sector;
  LayerHandle m_layer;
  std::shared_ptr<NavGraphEdgeObjectModel> m_edge;

public:
  NavGraphEdgeAddCommand(SectorModel& sector,
                         LayerHandle layer,
                         std::shared_ptr<NavGraphEdgeObjectModel> edge)
    : m_sector(sector),
      m_layer(layer),
      m_edge(edge)
  {}

  void redo() override
  {
    if (m_layer)
      m_layer->add(m_edge);
    else
      std::cout << "NavGraphEdgeAddCommand: null layer" << std::endl;

    m_sector.get_nav_graph().add_edge(m_edge);
  }

  void undo() override
  {
    m_sector.get_nav_graph().remove_edge(m_edge);
    m_layer->remove(m_edge);
  }
};

class NavGraphEdgeRemoveCommand : public Command
{
private:
  SectorModel& m_sector;
  LayerHandle m_layer;
  std::shared_ptr<NavGraphEdgeObjectModel> m_edge;

public:
  NavGraphEdgeRemoveCommand(SectorModel& sector,
                            std::shared_ptr<NavGraphEdgeObjectModel> edge)
    : m_sector(sector),
      m_layer(sector.get_layer(edge)),
      m_edge(edge)
  {}

  void redo() override
  {
    if (m_layer)
      m_layer->remove(m_edge);
    else
      std::cout << "NavGraphEdgeRemoveCommand: null layer" << std::endl;

    m_sector.get_nav_graph().remove_edge(m_edge);
  }

  void undo() override
  {
    m_sector.get_nav_graph().add_edge(m_edge);
    if (m_layer)
      m_layer->add(m_edge);
  }
};

} // namespace windstille

#endif

/* EOF */
