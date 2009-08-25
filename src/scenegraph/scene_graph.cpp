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

#include "scenegraph/scene_graph.hpp"

#include "display/drawing_request.hpp"

SceneGraph::SceneGraph()
  : m_drawing_requests()
{
}

void
SceneGraph::add_drawable(boost::shared_ptr<DrawingRequest> drawable)
{
  m_drawing_requests.push_back(drawable);
}

void
SceneGraph::remove_drawable(boost::shared_ptr<DrawingRequest> /*drawable*/)
{
  // FIXME: implement me
}

void
SceneGraph::draw(const Texture& tmp_texture)
{
  for(DrawingRequests::iterator i = m_drawing_requests.begin(); i != m_drawing_requests.end(); ++i)
  {
    (*i)->draw(tmp_texture);
  }
}

/* EOF */
