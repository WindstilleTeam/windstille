/*  $Id$
**   __      __ __             ___        __   __ __   __
**  /  \    /  \__| ____    __| _/_______/  |_|__|  | |  |   ____
**  \   \/\/   /  |/    \  / __ |/  ___/\   __\  |  | |  | _/ __ \
**   \        /|  |   |  \/ /_/ |\___ \  |  | |  |  |_|  |_\  ___/
**    \__/\  / |__|___|  /\____ /____  > |__| |__|____/____/\___  >
**         \/          \/      \/    \/                         \/
**  Copyright (C) 2005 Ingo Ruhnke <grumbel@gmx.de>
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

#ifndef HEADER_SHOCKWAVE_HPP
#define HEADER_SHOCKWAVE_HPP

#include "display/shader_program.hpp"
#include "display/texture.hpp"
#include "game_object.hpp"

/** */
class Shockwave : public GameObject
{
private:
  Vector        pos;
  Texture       noise;
  ShaderProgram shader_program;
  float radius;

public:
  Shockwave(FileReader& props);
  ~Shockwave();

  void draw (SceneContext& context);
  void update (float delta);

private:
  Shockwave (const Shockwave&);
  Shockwave& operator= (const Shockwave&);
};

#endif

/* EOF */
