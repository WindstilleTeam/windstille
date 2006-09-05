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

#include "random.hpp"
#include "vrdummy.hpp"

VRDummy::VRDummy(FileReader& props)
{
  props.get("name", name);
  props.get("pos",  pos);
  props.print_unused_warnings("VRDummy");
  
  sprite = Sprite3D("3dsprites/vrdummy.wsprite");
  rotation = 0;

  highlight = Sprite("images/hedgehog_highlight.sprite");
  highlight.set_blend_func(GL_SRC_ALPHA, GL_ONE);
  
  jump_time = 0;
}

VRDummy::~VRDummy()
{
  
}

void
VRDummy::draw(SceneContext& sc)
{
  sc.highlight().push_modelview();
  sc.highlight().translate(pos.x, pos.y);
  sc.highlight().rotate(rotation, 0.0f, 1.0f, 0.0f);
  sprite.draw(sc.highlight(), Vector(0, 0), 1200.0f);
  sc.highlight().pop_modelview();

  sc.highlight().draw(highlight, pos, 1500.0f);
}

void
VRDummy::update(float delta)
{
  sprite.update(delta);
  rotation += delta * 90.0f;
  jump_time += delta;
  
  if (jump_time >= 1.0f && velocity == Vector(0, 0)) 
    {
      velocity = Vector(rnd.drand(-300.0f, 300.0f),
                        rnd.drand(-300.0f, 300.0f));
    }

  if (jump_time >= 1.5f)
    {
      jump_time = 0.0f;
      
      velocity = Vector(0, 0);
    }

  pos += velocity * delta;
}

/* EOF */
