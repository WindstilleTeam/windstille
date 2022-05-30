/*
**  Windstille - A Sci-Fi Action-Adventure Game
**  Copyright (C) 2000 Ingo Ruhnke <grumbel@gmail.com>
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

#include "app/app.hpp"
#include "engine/sector.hpp"
#include "engine/script_manager.hpp"
#include "app/console.hpp"
#include "sprite3d/manager.hpp"

namespace windstille {

Character::Character(ReaderMapping const& props) :
  sprite(),
  z_pos(100.0f)
{
  set_useable(true);
  use_verb = "Talk";

  std::string sprite3d_filename = "models/characters/jane/jane.wsprite";
  std::string action_name;

  props.read("name", name);
  props.read("pos", pos);
  props.read("z-pos", z_pos);
  props.read("sprite3d", sprite3d_filename);
  props.read("action", action_name);

  sprite = g_app.sprite3d().create(Pathname(sprite3d_filename));

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
Character::draw (wstdisplay::SceneContext& sc)
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

} // namespace windstille

/* EOF */
