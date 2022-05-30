/*
**  Windstille - A Sci-Fi Action-Adventure Game
**  Copyright (C) 2010 Ingo Ruhnke <grumbel@gmail.com>
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

#include "slide_path.hpp"

#include <iostream>
#include <assert.h>
#include <math.h>
#include <stdexcept>

SlidePath::SlidePath() :
  m_nodes(),
  m_edges()
{
}

void
SlidePath::add_node(glm::vec2 const& pos, float zoom)
{
  SlidePathNode node;
  node.pos  = pos;
  node.zoom = zoom;
  m_nodes.push_back(node);
}

void
SlidePath::add_edge(float duration)
{
  SlidePathEdge edge;
  edge.duration = duration;
  m_edges.push_back(edge);
}

SlidePathNode
SlidePath::get(float relative_time)
{
  assert(m_nodes.size() == m_edges.size()+1);

  if (m_nodes.empty())
  {
    return SlidePathNode();
  }
  else
  {
    int idx = -1;
    float passed = 0.0f;
    for(size_t i = 0; i < m_edges.size(); ++i)
    {
      passed += m_edges[i].duration;
      if (passed > relative_time)
      {
        idx = static_cast<int>(i);
        break;
      }
    }

    if (idx == -1)
    {
      std::cout << "XXX BUMMER: not implemented" << std::endl;
      SlidePathNode node;
      return node;
    }
    else
    {
      SlidePathNode lhs = m_nodes[idx];
      SlidePathNode rhs = m_nodes[idx+1];

      float t = relative_time - (passed - m_edges[idx].duration);

      float factor = t / m_edges[idx].duration;

      SlidePathNode node;
      node.pos  = lhs.pos  * (1.0f - factor) + rhs.pos  * factor;
      node.zoom = lhs.zoom * (1.0f - factor) + rhs.zoom * factor;
      return node;
    }
  }
}

void
SlidePath::finish()
{
  if (m_nodes.empty())
  {
    throw std::runtime_error("nodes empty");
  }
  else if (m_edges.empty())
  {
    throw std::runtime_error("edges empty");
  }
  else if (m_nodes.size() == m_edges.size())
  {
    m_nodes.push_back(m_nodes.back());
  }
}

float
SlidePath::length() const
{
  float total = 0.0f;
  for(Edges::const_iterator i = m_edges.begin(); i != m_edges.end(); ++i)
  {
    total += i->duration;
  }
  return total;
}

/* EOF */
