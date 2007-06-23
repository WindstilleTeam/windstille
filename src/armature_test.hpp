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

#ifndef HEADER_ARMATURE_TEST_HPP
#define HEADER_ARMATURE_TEST_HPP

#include "armature/armature.hpp"
#include "armature/model.hpp"
#include "screen.hpp"

/** */
class ArmatureTest : public Screen
{
private:
  Model* model;
  Armature* armature;
  std::vector<Pose*> poses;
  
  int pose_idx;
  float time;

  float xrot;
  float yrot;
  float zrot;

public:
  ArmatureTest();

  void draw();
  void update(float delta, const Controller& controller);

private:
  ArmatureTest (const ArmatureTest&);
  ArmatureTest& operator= (const ArmatureTest&);
};

#endif

/* EOF */
