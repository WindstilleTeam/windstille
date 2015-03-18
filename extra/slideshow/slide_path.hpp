/*
**  Windstille - A Sci-Fi Action-Adventure Game
**  Copyright (C) 2010 Ingo Ruhnke <grumbel@gmx.de>
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

#ifndef HEADER_WINDSTILLE_EXTRA_SLIDESHOW_SLIDE_PATH_HPP
#define HEADER_WINDSTILLE_EXTRA_SLIDESHOW_SLIDE_PATH_HPP

#include <vector>

#include "math/vector2f.hpp"

class SlidePathNode
{
public:
  Vector2f  pos;
  float     zoom;

  SlidePathNode() :
    pos(0.0f, 0.0f),
    zoom(1.0f)
  {}
};

class SlidePathEdge
{
public:
  float duration;
};

class SlidePath
{
private:
  typedef std::vector<SlidePathNode> Nodes;
  typedef std::vector<SlidePathEdge> Edges;

  Nodes m_nodes;
  Edges m_edges;

public:
  SlidePath();

  SlidePathNode get(float relative_time);

  void add_node(const Vector2f& pos, float zoom);
  void add_edge(float duration);
  void finish();

  float length() const;

private:
  SlidePath(const SlidePath&);
  SlidePath& operator=(const SlidePath&);
};

#endif

/* EOF */
