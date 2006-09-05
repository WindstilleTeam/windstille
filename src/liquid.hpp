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

#ifndef HEADER_WINDSTILLE_LIQUID_HPP
#define HEADER_WINDSTILLE_LIQUID_HPP

#include <vector>
#include "lisp/lisp.hpp"
#include "entity.hpp"

/** */
class Liquid : public Entity
{
private:
  Texture texture;
  float t;

  std::vector<float> heightfield_store1;
  std::vector<float> heightfield_store2;

  std::vector<float>* heightfield1;
  std::vector<float>* heightfield2;

public:
  Liquid(FileReader& props);
  ~Liquid();

  void draw(SceneContext& sc);
  void update(float delta);

private:
  Liquid (const Liquid&);
  Liquid& operator= (const Liquid&);
};

#endif

/* EOF */
