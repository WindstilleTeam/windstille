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

int g_stencil_enabled = 0;

StencilDrawable::StencilDrawable() :
  m_stencil_group(),
  m_drawable_group()
{
}

void
StencilDrawable::render(unsigned int mask)
{
  if (g_stencil_enabled == 0)
  {
    g_stencil_enabled = 1;

    glEnable(GL_STENCIL_TEST);

    // enable stencil and clear it
    glClearStencil(0);
    glClear(GL_STENCIL_BUFFER_BIT);
  }
  else
  {
    g_stencil_enabled += 1;
  }

  // glStencilFunc: set when the test passes or fails 
  // glStencilOp: set what is done when the test passes/fails

  // render stencil buffer content
  glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
  glStencilFunc(GL_ALWAYS, 1, 1);
  glStencilOp(GL_KEEP, GL_KEEP, GL_INCR);

  glEnable(GL_ALPHA_TEST);
  glAlphaFunc(GL_GREATER, 0.5f);
  m_stencil_group.render(~0u);
  glDisable(GL_ALPHA_TEST);

  // render framebuffer content
  glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
  glStencilFunc(GL_EQUAL, g_stencil_enabled, g_stencil_enabled);
  glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);

  m_drawable_group.render(~0u);

  g_stencil_enabled -= 1;

  if (g_stencil_enabled == 0)
  {
    // disable stencil and reset op and func
    glStencilFunc(GL_ALWAYS, 0, 1);
    glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);

    glDisable(GL_STENCIL_TEST);
  }
}

/* EOF */
