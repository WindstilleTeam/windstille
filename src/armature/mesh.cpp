/*  $Id$
**   __      __ __             ___        __   __ __   __
**  /  \    /  \__| ____    __| _/_______/  |_|__|  | |  |   ____
**  \   \/\/   /  |/    \  / __ |/  ___/\   __\  |  | |  | _/ __ \
**   \        /|  |   |  \/ /_/ |\___ \  |  | |  |  |_|  |_\  ___/
**    \__/\  / |__|___|  /\____ /____  > |__| |__|____/____/\___  >
**         \/          \/      \/    \/                         \/
**  Copyright (C) 2007 Ingo Ruhnke <grumbel@gmx.de>
**
**  This program is free software; you can redistribute it and/or
**  modify it under the terms of the GNU General Public License
**  as published by the Free Software Foundation; either version 2
**  of the License, or (at your option) any later version.
**
**  This program is distributed in the hope that it will be useful,
**  but WITHOUT ANY WARRANTY; without even the implied warranty of
**  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
**  GNU General Public License for more details.
** 
**  You should have received a copy of the GNU General Public License
**  along with this program; if not, write to the Free Software
**  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA
**  02111-1307, USA.
*/

#include <assert.h>
#include <stdexcept>
#include <iostream>
#include "util.hpp"
#include "display/opengl_state.hpp"
#include "file_reader.hpp"
#include "mesh.hpp"

Mesh::Mesh(FileReader& reader)
  : blend_sfactor(GL_ONE),
    blend_dfactor(GL_ZERO)
{
  if (reader.get_name() != "mesh")
    throw std::runtime_error("Not a 'mesh' type, its '" + reader.get_name() + "'");
  
  reader.get("name",      name);
  reader.get("vertices",  vertices);
  reader.get("normals",   normals);
  reader.get("texcoords", texcoords);
  reader.get("triangles", triangles);
  
  // FIXME: add 'influences' tag parsing

  // Check that all vectors have the same right modulo
  assert(vertices.size()  % 3 == 0);
  assert(normals.size()   % 3 == 0);
  assert(texcoords.size() % 2 == 0);

  // Check that all vectors contain enough values for the given number
  // of vertices
  assert(vertices.size()/3 == normals.size()/3);
  assert(vertices.size()/3 == texcoords.size()/2);

  std::cout << "Number of normals:   " << normals.size()   << std::endl;
  std::cout << "Number of texcoords: " << texcoords.size() << std::endl;
  std::cout << "Number of vertices:  " << vertices.size()  << std::endl;
  std::cout << "Number of triangles: " << triangles.size() << std::endl;
}

Mesh::~Mesh()
{
  
}

void
Mesh::draw()
{
  std::cout << "Mesh: Drawing: " << vertices.size() << std::endl;
  OpenGLState state;

  if (blend_sfactor != GL_ONE || blend_dfactor != GL_ZERO)
    {
      state.enable(GL_BLEND);
      state.set_blend_func(blend_sfactor, blend_dfactor);
    }
  else
    {
      state.enable(GL_DEPTH_TEST);
    }
  
  glLineWidth(1.0f);
  glColor4f(1.0f, 1.0f, 1.0f, 0.5f);
  //state.enable_client_state(GL_VERTEX_ARRAY);
  //state.enable_client_state(GL_NORMAL_ARRAY);
  //state.enable_client_state(GL_TEXTURE_COORD_ARRAY);  
  state.activate();

  assert_gl("gl init before sprite");

  //  glVertexPointer(3, GL_FLOAT, 0, &*vertices.begin());
  //glNormalPointer(GL_FLOAT, 0, &*normals.begin());
  //glTexCoordPointer(2, GL_FLOAT, 0, &*texcoords.begin());
  //glDrawElements(GL_LINE_STRIP, triangles.size(), GL_INT, &*triangles.begin());
  glBegin(GL_TRIANGLES);
  for(int i = 0; i < int(triangles.size()); i += 3)
    {
      glVertex3f(vertices[i+0], 
                 vertices[i+1],
                 vertices[i+2]);
    }
  glEnd();
}

/* EOF */
