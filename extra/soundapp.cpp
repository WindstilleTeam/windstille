/*
**  Windstille - A Sci-Fi Action-Adventure Game
**  Copyright (C) 2010 Ingo Ruhnke <grumbel@gmx.de>
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

#include <iostream>

#include "sound/sound_manager.hpp"
#include "sound/sound_source.hpp"

int main(int argc, char** argv)
{
  if (argc != 3)
  {
    std::cout << "Usage: " << argv[0] << " SOUND VOICE" << std::endl;
    return 1;
  }
  else
  {
    SoundManager sound_manager;
  
    Vector2f pos(0.0f, 0.0f);
    SoundSourcePtr source = sound_manager.sound().prepare(Pathname(argv[1], Pathname::kSysPath));
    SoundSourcePtr voice  = sound_manager.voice().prepare(Pathname(argv[2], Pathname::kSysPath));

    sound_manager.set_master_volume(10.0f);
    sound_manager.sound().set_volume(0.1f);
    sound_manager.voice().set_volume(1.0f);
    
    if (true)
    {
      source->set_looping(true);
      source->set_position(pos);
      //source->set_velocity(Vector2f(5.0f, 0.0f));
      source->play();
    }

    if (true)
    {
      voice->set_looping(true);
      voice->set_position(pos);
      voice->play();
    }

    while(source->is_playing())
    {
      pos.x += 25.0f;
      //source->set_position(pos);
      usleep(100000);
      std::cout << "." << std::flush;
    }
  
    return 0;
  }
}

/* EOF */
