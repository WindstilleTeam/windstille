//
// 
//  Windstille - A Jump'n Shoot Game
//  Copyright (C) 2000 Ingo Ruhnke <grumbel@gmx.de>
//
//  This program is free software; you can redistribute it and/or
//  modify it under the terms of the GNU General Public License
//  as published by the Free Software Foundation; either version 2
//  of the License, or (at your option) any later version.
//
//  This program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
// 
//  You should have received a copy of the GNU General Public License
//  along with this program; if not, write to the Free Software
//  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.

#include "character.hpp"
#include "sector.hpp"
#include "game_session.hpp"
#include "scripting/wrapper.interface.hpp"
#include "dialog_manager.hpp"
#include "script_manager.hpp"
#include "physfs/physfs_stream.hpp"
#include "console.hpp"
#include "sprite3d/manager.hpp"
#include "util.hpp"

#include <exception>

Character::Character(FileReader& props)
{
  set_useable(true);
  use_verb = "Talk";
  z_pos = 100.0f;

  std::string sprite3d_filename = "models/characters/jane/jane.wsprite";
  std::string action_name;

  props.get("name", name);
  props.get("pos", pos);
  props.get("z-pos", z_pos);
  props.get("sprite3d", sprite3d_filename);
  props.get("action", action_name);
  props.print_unused_warnings("character");
  
  sprite = Sprite3D(sprite3d_filename);

  if (!action_name.empty())
    sprite.set_action(action_name);
}

Character::~Character()
{
}

void
Character::update(float delta)
{   
  sprite.update(delta);
}

void
Character::draw (SceneContext& sc)
{
  sprite.draw(sc.color(), pos, z_pos);
}

void
Character::use()
{
  std::string filename = basename(Sector::current()->get_filename());
  filename.erase(filename.find('.'));
  filename = "scripts/" + filename + "/" + name + ".nut";

  try 
    {
      script_manager->run_script_file(filename);
    } catch (std::exception& e) {
      console << e.what() << std::endl;
    }
}

/* EOF */
