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

#include "sound/ogg_sound_file.hpp"

#include "util/pathname.hpp"

OggSoundFile::OggSoundFile(const Pathname& filename) :
  m_in(filename.get_sys_path().c_str(), std::ios::binary),
  vorbis_file()
{
  ov_callbacks callbacks = { cb_read, cb_seek, cb_close, cb_tell };
  ov_open_callbacks(&m_in, &vorbis_file, 0, 0, callbacks);

  vorbis_info* vi = ov_info(&vorbis_file, -1);
  channels = vi->channels;
  rate = vi->rate;
  bits_per_sample = 16;
  size = static_cast<size_t> (ov_pcm_total(&vorbis_file, -1) * 2);
}

OggSoundFile::~OggSoundFile()
{
  ov_clear(&vorbis_file);
}

size_t
OggSoundFile::read(void* _buffer, size_t buffer_size)
{
  char* buffer = reinterpret_cast<char*> (_buffer);
  int section = 0;
  size_t totalBytesRead= 0;

  while(buffer_size>0){
    long bytesRead 
      = ov_read(&vorbis_file, buffer, static_cast<int> (buffer_size), 0, 2, 1,
                &section);
    if(bytesRead==0){
      break;
    }
    buffer_size -= bytesRead;
    buffer += bytesRead;
    totalBytesRead += bytesRead;
  }
  
  return totalBytesRead;
}

void
OggSoundFile::reset()
{
  ov_raw_seek(&vorbis_file, 0);
}

size_t
OggSoundFile::cb_read(void* ptr, size_t size, size_t nmemb, void* source)
{
  std::ifstream& in = *reinterpret_cast<std::ifstream*>(source);
  if (!in.read(static_cast<char*>(ptr), size * nmemb))
  {
    return -1;
  }
  else
  {
    return in.gcount();
  }
}

int
OggSoundFile::cb_seek(void* source, ogg_int64_t offset, int whence)
{
  switch(whence) 
  {
    case SEEK_SET:
      if (!reinterpret_cast<std::ifstream*>(source)->seekg(offset, std::ios::beg))
        return -1;
      break;

    case SEEK_CUR:
      if (!reinterpret_cast<std::ifstream*>(source)->seekg(offset, std::ios::cur))
        return -1;
      break;

    case SEEK_END:
      if (!reinterpret_cast<std::ifstream*>(source)->seekg(offset, std::ios::end))
        return -1;
      break;

    default:
      return -1;
  }
  return 0;
}
  
int
OggSoundFile::cb_close(void* source)
{
  reinterpret_cast<std::ifstream*>(source)->close();
  return 0;
}

long
OggSoundFile::cb_tell(void* source)
{
  return reinterpret_cast<std::ifstream*>(source)->tellg();
}

/* EOF */
