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

#include "math/random.hpp"
#include "sound/sound_manager.hpp"
#include "sound/sound_source.hpp"
#include "sound/sound_file.hpp"
#include "sound/filtered_sound_file.hpp"

int main(int argc, char** argv)
{
  if (argc < 2)
  {
    std::cout << "Usage: " << argv[0] << " SOUNDS..." << std::endl;
    return 1;
  }
  else
  {
    SoundManager sound_manager;

    sound_manager.set_gain(1.0f);
    sound_manager.sound().set_gain(1.0f);
    sound_manager.voice().set_gain(1.0f);

    Random random(time(NULL));

    std::cout << "Filter Test" << std::endl;

    std::vector<SoundSourcePtr> sources;
    for(int i = 1; i < argc; ++i)
    {
      std::unique_ptr<SoundFile> sound_file = SoundFile::load(Pathname(argv[i], Pathname::kSysPath));
      //std::unique_ptr<SoundFile> filtered_sound_file(new FilteredSoundFile(sound_file));
      SoundSourcePtr source = sound_manager.sound().prepare(std::move(sound_file), kStreamSoundSource);

      source->set_looping(true);
      Vector2f pos(random.frand(-500, 500), 0.0f);
      source->set_position(pos);
      //source->set_rolloff_factor(0.0f);

      std::cout << argv[i] << ": " << pos << std::endl;

      sources.push_back(source);
    }

    for(std::vector<SoundSourcePtr>::iterator i = sources.begin(); i != sources.end(); ++i)
    {
      (*i)->play();
    }

    while(true)
    {
      for(std::vector<SoundSourcePtr>::iterator i = sources.begin(); i != sources.end(); ++i)
      {
        std::cout << "pos: " << (*i)->get_pos() << std::endl;
      }

      usleep(10000);

      sound_manager.update(100);
    }

    return 0;
  }
}

/* EOF */
