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

#include <string.h>
#include <stdexcept>
#include <stdio.h>
#include <stdint.h>

#include "sound/wav_sound_file.hpp"
#include "util/pathname.hpp"
#include "util/util.hpp"

WavSoundFile::WavSoundFile(const Pathname& filename) :
  file(),
  datastart()
{
  file.open(filename.get_sys_path().c_str(), std::ios::binary);
  if (!file)
  {
    std::ostringstream str;
    str << "WavSoundFile(): Couldn't open " << filename;
    throw std::runtime_error(str.str());
  }

  char magic[4];
  if (!file.read(magic, sizeof(magic)))
  {
    throw std::runtime_error("Couldn't read file magic (not a wave file)");
  }

  if(strncmp(magic, "RIFF", 4) != 0) 
  {
    printf("MAGIC: %4s.\n", magic);
    throw std::runtime_error("file is not a RIFF wav file");
  }

  uint32_t wavelen = read_uint32_t(file);
  
  if (!file.read( magic, sizeof(magic)))
  {
    throw std::runtime_error("Couldn't read chunk header (not a wav file?)");
  }

  if(strncmp(magic, "WAVE", 4) != 0)
  {
    throw std::runtime_error("file is not a valid RIFF/WAVE file");
  }

  char chunkmagic[4];
  uint32_t chunklen;

  // search audio data format chunk
  do {
    if (!file.read(chunkmagic, sizeof(chunkmagic)))
    {
      throw std::runtime_error("EOF while searching format chunk");    
    }
    chunklen = read_uint32_t(file);
    
    if (strncmp(chunkmagic, "fmt ", 4) == 0)
    {
      break;
    }
    else if (strncmp(chunkmagic, "fact", 4) == 0 ||
             strncmp(chunkmagic, "LIST", 4) == 0)
    {
      // skip chunk

      if (!file.seekg(chunklen, std::ios::cur))
      {
        throw std::runtime_error("EOF while searching fmt chunk");
      }
    }
    else 
    {
      throw std::runtime_error("complex WAVE files not supported");
    }
  } while(true); 

  if (chunklen < 16)
    throw std::runtime_error("Format chunk too short");
 
  // parse format
  uint16_t encoding = read_uint16_t(file);
  if (encoding != 1)
  {
    std::ostringstream str;
    str << "WavSoundFile(): only PCM encoding supported, got " << encoding;
    throw std::runtime_error(str.str());
  }
  channels = read_uint16_t(file);
  rate = read_uint32_t(file);
  uint32_t byterate = read_uint32_t(file);
  uint16_t blockalign = read_uint16_t(file);
  bits_per_sample = read_uint16_t(file);

  if(chunklen > 16) 
  {
    if(file.seekg(chunklen-16, std::ios::cur) == 0)
      throw std::runtime_error("EOF while reading reast of format chunk");
  }

  // set file offset to DATA chunk data
  do {
    if (!file.read(chunkmagic, sizeof(chunkmagic)))
      throw std::runtime_error("EOF while searching data chunk");    
    chunklen = read_uint32_t(file);

    if(strncmp(chunkmagic, "data", 4) == 0)
      break;

    // skip chunk
    if(file.seekg(chunklen, std::ios::cur) == 0)
      throw std::runtime_error("EOF while searching fmt chunk");
  } while(true);

  datastart = file.tellg();
  size = static_cast<size_t> (chunklen);
}

WavSoundFile::~WavSoundFile()
{
}

void
WavSoundFile::reset()
{
  if (!file.seekg(datastart))
    throw std::runtime_error("Couldn't seek to data start");
}

size_t
WavSoundFile::read(void* buffer, size_t buffer_size)
{
  size_t end = datastart + size;
  size_t cur = file.tellg();

  if (cur >= end)
    return 0;
  
  size_t readsize = std::min(static_cast<size_t> (end - cur), buffer_size);

  if (!file.read(static_cast<char*>(buffer), readsize))
  {
    throw std::runtime_error("read error while reading samples");
  }

  return readsize;
}

/* EOF */
