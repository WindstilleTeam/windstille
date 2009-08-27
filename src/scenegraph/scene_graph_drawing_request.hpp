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

#ifndef HEADER_WINDSTILLE_SCENEGRAPH_SCENE_GRAPH_DRAWING_REQUEST_HPP
#define HEADER_WINDSTILLE_SCENEGRAPH_SCENE_GRAPH_DRAWING_REQUEST_HPP

#include "scenegraph/scene_graph.hpp"
#include "display/drawing_request.hpp"

class SceneGraphDrawingRequest : public DrawingRequest
{
private:
  SceneGraph& m_sg;

public:
  SceneGraphDrawingRequest(SceneGraph& sg, const Matrix& modelview_)
    : DrawingRequest(Vector2f(), 1000, modelview_),
      m_sg(sg)
  {}

  void draw(const Texture& tmp_texture)
  {
    glPushMatrix();
    glMultMatrixf(modelview.matrix);

    m_sg.draw(tmp_texture);

    glPopMatrix();
  }
};

#endif

/* EOF */
