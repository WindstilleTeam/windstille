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

#ifndef HEADER_WINDSTILLE_EDITOR_NAVGRAPH_COMMANDS_HPP
#define HEADER_WINDSTILLE_EDITOR_NAVGRAPH_COMMANDS_HPP

#include "editor/command.hpp"

class NavGraphNodeAddCommand : public Command
{
private:
  boost::shared_ptr<NavGraphNodeObjectModel> m_node;

public:
  NavGraphNodeAddCommand(boost::shared_ptr<NavGraphNodeObjectModel> node)
    : m_node(node)
  {}

  void redo() 
  {
  }

  void undo() 
  {
  }
};

class NavGraphNodeRemoveCommand : public Command
{
private:
  boost::shared_ptr<NavGraphNodeObjectModel> m_node;

public:
  NavGraphNodeRemoveCommand(boost::shared_ptr<NavGraphNodeObjectModel> node)
    : m_node(node)
  {}

  void redo() 
  {
  }

  void undo() 
  {
  }
};

class NavGraphEdgeAddCommand : public Command
{
private:
  LayerHandle m_layer;
  boost::shared_ptr<NavGraphNodeObjectModel> m_lhs;
  boost::shared_ptr<NavGraphNodeObjectModel> m_rhs;

public:
  NavGraphEdgeAddCommand(LayerHandle layer, 
                         boost::shared_ptr<NavGraphNodeObjectModel> lhs,
                         boost::shared_ptr<NavGraphNodeObjectModel> rhs)
    : m_layer(layer),
      m_lhs(lhs),
      m_rhs(rhs)
  {}

  void redo() 
  {
  }

  void undo() 
  {
  }
};

class NavGraphEdgeRemoveCommand : public Command
{
private:
  boost::shared_ptr<NavGraphEdgeObjectModel> m_edge;

public:
  NavGraphEdgeRemoveCommand(boost::shared_ptr<NavGraphEdgeObjectModel> edge)
    : m_edge(edge)
  {}

  void redo() 
  {
  }

  void undo() 
  {
  }
};

#endif

/* EOF */
