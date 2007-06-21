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

#include <GL/glew.h>
#include <GL/gl.h>
#include "input/controller.hpp"
#include "display/display.hpp"
#include "screen_manager.hpp"
#include "display/display.hpp"
#include "armature_test.hpp"

ArmatureTest::ArmatureTest()
{
  FileReader reader = FileReader::parse("armature.arm");
  armature = new Armature(reader);
  
  xrot = 0;
  yrot = 0;
}

void
ArmatureTest::draw()
{
  glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  glPushMatrix();

  glTranslatef(400.0f, 300.0f, 0.0f);
  glScalef(64.0f, 64.0f, 64.0f);
 
  glRotatef(xrot, 1.0f, 0.0f, 0.0f);
  glRotatef(yrot, 0.0f, 1.0f, 0.0f);
  glRotatef(zrot, 0.0f, 0.0f, 1.0f);

  armature->draw();

  glPopMatrix();

  // std::cout << xrot << " " << yrot << std::endl;
}

void
ArmatureTest::update(float delta, const Controller& controller)
{
  if (controller.button_was_pressed(ESCAPE_BUTTON) ||
      controller.button_was_pressed(PAUSE_BUTTON))
    {
      screen_manager.pop_screen();
    }

  if (controller.button_was_pressed(PRIMARY_BUTTON))
    {
      xrot = 90;
      yrot = 0;
      zrot = 0;
    }
  else if (controller.button_was_pressed(SECONDARY_BUTTON))
      {
      xrot = 0;
      yrot = 90;
      zrot = 0;
      }
  if (controller.button_was_pressed(TERTIARY_BUTTON))
    {
      xrot = 0;
      yrot = 0;
      zrot = 90;
    }
  else
    { 
      xrot += controller.get_axis_state(X_AXIS) * 90 * delta;
      yrot += controller.get_axis_state(Y_AXIS) * 90 * delta;
      zrot += controller.get_axis_state(X2_AXIS) * 90 * delta;
    }
}

/* EOF */
