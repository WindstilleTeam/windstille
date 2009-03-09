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

#include "sound_file.hpp"

#include <string.h>
#include <stdio.h>
#include <stdint.h>
#include <algorithm>
#include <stdexcept>
#include <sstream>
#include <assert.h>
#include <physfs.h>

#include "ogg_sound_file.hpp"
#include "wav_sound_file.hpp"

#include <fstream>

SoundFile*
SoundFile::load(const std::string& filename)
{
  PHYSFS_file* file = PHYSFS_openRead(filename.c_str());

  if (!file) 
    {
      std::stringstream msg;
      msg << "Couldn't open '" << filename << "': " << PHYSFS_getLastError();
      throw std::runtime_error(msg.str());
    }
  else
    {
      try {
        char magic[4];

        if (PHYSFS_read(file, magic, sizeof(magic), 1) != 1)
          {
            throw std::runtime_error("Couldn't read magic, file too short");
          }
        else
          {
            PHYSFS_seek(file, 0);

            if (strncmp(magic, "RIFF", 4) == 0)
              {
                return new WavSoundFile(file);
              }
            else if (strncmp(magic, "OggS", 4) == 0)
              {
                return new OggSoundFile(file);
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

/* EOF */
