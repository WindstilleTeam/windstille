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

#include "objects/trigger.hpp"

#include "engine/sector.hpp"
#include "objects/player.hpp"

Trigger::Trigger(const FileReader& props)
  : area(),
    callback(),
    triggered(false), 
    last_trigger(),
    one_time_trigger(false)
{
  float x = -1;
  float y = -1;
  float width = -1;
  float height = -1;
  
  props.get("x", x);
  props.get("y", y);
  props.get("width", width);
  props.get("height", height);
  props.get("callback", callback);
  props.get("one-time-trigger", one_time_trigger);
  
  if(x < 0 || y < 0 || width < 0 || height < 0)
    throw std::runtime_error("Invalid or missing area in Trigger object");
 
  area.left = x;
  area.top = y;
  area.right = area.left + width;
  area.bottom = area.top + height;
}

Trigger::~Trigger()
{
}

void
Trigger::draw (SceneContext& )
{
}

void
Trigger::update(float /*delta*/)
{
  //FIXME use proper collision detection
  Player* player = Player::current();
  if(!area.is_inside(player->get_pos())) 
    {
      last_trigger = false;
      return;
    }
  
  if(triggered && one_time_trigger)
    return;

  if(last_trigger == false) 
    {
      triggered = true;
      try 
        {
          Sector::current()->call_script_function(callback);
        }
      catch(std::exception& e) 
        {
          std::cerr << "Couldn't run trigger-script: " << e.what() << "\n";
        }
    }
  last_trigger = true;
}

/* EOF */
