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

#include "stencil_drawable.hpp"

StencilDrawable::StencilDrawable() :
  m_stencil_group(),
  m_drawable_group()
{
}

void
StencilDrawable::render(unsigned int mask)
{
  // FIXME: untested code

  // FIXME: this wouldn't work with recursive stencils, maybe solvable
  // by doing some cleverer stuff with glStencilFunc and glStencilOp
  // (i.e. increment on each recursion and then test on the recursion
  // depth count)

  /*
    http://www.opengl.org/documentation/specs/man_pages/hardcopy/GL/html/gl/stencilfunc.html
    http://www.opengl.org/documentation/specs/man_pages/hardcopy/GL/html/gl/stencilop.html
  */

  // enable stencil and clear it
  glEnable(GL_STENCIL_TEST);
  glClearStencil(0);

  // glStencilFunc: set when the test passes or fails 
  // glStencilOp: set what is done when the test passes/fails

  // render stencil buffer content
  glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
  glStencilFunc(GL_ALWAYS, 1, 1);
  glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
  m_stencil_group.render(~0u);
  
  // render framebuffer content
  glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
  glStencilFunc(GL_EQUAL, 1, 1);
  glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);
  m_drawable_group.render(~0u);

  // disable stencil and reset op and func
  glStencilFunc(GL_ALWAYS, 0, 1);
  glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);
  glDisable(GL_STENCIL_TEST);
}

/* EOF */
