/*
**  Windstille - A Sci-Fi Action-Adventure Game
**  Copyright (C) 2005 Matthias Braun <matze@braunis.de>
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

/** Used SDL_mixer and glest source as reference */

#include <string.h>
#include <stdexcept>
#include <sstream>
#include <fstream>

#include "sound/ogg_sound_file.hpp"
#include "sound/wav_sound_file.hpp"
#include "util/pathname.hpp"

std::unique_ptr<SoundFile>
SoundFile::load(const Pathname& filename)
{
  std::ifstream in(filename.get_sys_path().c_str(), std::ios::binary);

  if (!in) 
  {
    std::stringstream msg;
    msg << "Couldn't open '" << filename << "'";
    throw std::runtime_error(msg.str());
  }
  else
  {
    try 
    {
      char magic[4];

      if (!in.read(magic, sizeof(magic)))
      {
        throw std::runtime_error("Couldn't read magic, file too short");
      }
      else
      {
        if (strncmp(magic, "RIFF", 4) == 0)
        {
          return std::unique_ptr<SoundFile>(new WavSoundFile(filename));
        }
        else if (strncmp(magic, "OggS", 4) == 0)
        {
          return std::unique_ptr<SoundFile>(new OggSoundFile(filename));
        }
        else
        {
          throw std::runtime_error("Unknown file format");
        }
      }
    } 
    catch(std::exception& e) 
    {
      std::stringstream msg;
      msg << "Couldn't read '" << filename << "': " << e.what();
      throw std::runtime_error(msg.str());
    }
  }
}

float
SoundFile::get_length() const
{
  return static_cast<float>(get_size()) 
    / static_cast<float>(get_rate())
    / static_cast<float>(get_channels())
    / (static_cast<float>(get_bits_per_sample())/8.0f);
}

/* EOF */
