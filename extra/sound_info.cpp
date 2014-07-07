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

#include "sound/sound_file.hpp"
#include "util/pathname.hpp"

int main(int argc, char** argv)
{
  if (argc < 2)
  {
    std::cout << "Usage: " << argv[0] << " FILE..." << std::endl;
    return 0;
  }
  else
  {
    for(int i = 1; i < argc; ++i)
    {
      std::unique_ptr<SoundFile> sound_file = SoundFile::load(Pathname(argv[i], Pathname::kSysPath));
      std::cout << "Filename: " << argv[i] << std::endl;
      std::cout << "Length:   " << sound_file->get_length() << "sec" << std::endl;
      std::cout << "Size:     " << sound_file->get_size() << "B" << std::endl;
      std::cout << "Rate:     " << sound_file->get_rate() << "Hz" << std::endl;
      std::cout << "Bits:     " << sound_file->get_bits_per_sample() << std::endl;
      std::cout << "Channels: " << sound_file->get_channels() << std::endl;
      std::cout << std::endl;
    }

    return 0;
  }
}

/* EOF */
