/*
**  Windstille - A Sci-Fi Action-Adventure Game
**  Copyright (C) 2000 Ingo Ruhnke <grumbel@gmx.de>
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

#include "objects/character.hpp"

#include "engine/sector.hpp"
#include "engine/script_manager.hpp"
#include "app/console.hpp"

Character::Character(const FileReader& props) :
  sprite(),
  z_pos(100.0f)
{
  set_useable(true);
  use_verb = "Talk";

  std::string sprite3d_filename = "models/characters/jane/jane.wsprite";
  std::string action_name;

  props.get("name", name);
  props.get("pos", pos);
  props.get("z-pos", z_pos);
  props.get("sprite3d", sprite3d_filename);
  props.get("action", action_name);
  
  sprite = Sprite3D(Pathname(sprite3d_filename));

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
  try 
  {
    Pathname path = Sector::current()->get_directory();
    path.append_path(name);
    path.append_text(".nut");

    ScriptManager::current()->run_script_file(path);
  }
  catch (std::exception& e) 
  {
    ConsoleLog << e.what() << std::endl;
  }
}

/* EOF */
