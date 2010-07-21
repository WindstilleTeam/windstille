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

#include <stdexcept>
#include <sstream>

#include "util/pathname.hpp"

OggSoundFile::OggSoundFile(const Pathname& filename) :
  m_in(),
  m_file_size(),
  m_vorbis_file(),
  m_channels(),
  m_rate(),
  m_bits_per_sample(),
  m_size()
{
  m_in.open(filename.get_sys_path().c_str(), std::ios::binary);
  if (!m_in)
  {
    std::ostringstream str;
    str << "OggSoundFile: Couldn't open: " << filename;
    throw std::runtime_error(str.str());
  }

  // get the file size
  m_in.seekg(0, std::ios::end);
  m_file_size = static_cast<size_t>(m_in.tellg());
  m_in.seekg(0, std::ios::beg);

  ov_callbacks callbacks = { cb_read, cb_seek, cb_close, cb_tell };

  int ret = ov_open_callbacks(this, &m_vorbis_file, 0, 0, callbacks);

  if (ret != 0)
  {
    std::ostringstream str;

    switch(ret)
    {
      case OV_EREAD:
        str << "OV_EREAD error while opening ogg";
        break;

      case OV_ENOTVORBIS:
        str << "OV_ENOTVORBIS error while opening ogg";
        break;

      case OV_EVERSION:
        str << "OV_EVERSION error while opening ogg";
        break;

      case OV_EBADHEADER:
        str << "OV_EBADHEADER error while opening ogg";
        break;

      case OV_EFAULT:
        str << "OV_EFAULT error while opening ogg";
        break;

      default:
        str << "Unknown error while opening ogg: " << ret;
        break;
    }

    throw std::runtime_error(str.str());
  }

  vorbis_info* vi = ov_info(&m_vorbis_file, -1);
  m_channels = vi->channels;
  m_rate = vi->rate;
  m_bits_per_sample = 16;
  m_size = static_cast<size_t> (ov_pcm_total(&m_vorbis_file, -1) * 2);
}

OggSoundFile::~OggSoundFile()
{
  ov_clear(&m_vorbis_file);
}

size_t
OggSoundFile::read(void* _buffer, size_t buffer_size)
{
  char* buffer = reinterpret_cast<char*> (_buffer);
  int section = 0;
  size_t totalBytesRead= 0;

  while(buffer_size>0){
    long bytesRead 
      = ov_read(&m_vorbis_file, buffer, static_cast<int> (buffer_size), 0, 2, 1,
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
  ov_raw_seek(&m_vorbis_file, 0);
}

void
OggSoundFile::seek_to(float sec)
{
  ov_time_seek(&m_vorbis_file, sec);
}

size_t
OggSoundFile::cb_read(void* ptr, size_t size, size_t nmemb, void* userdata)
{
  //std::cout << "OggSoundFile::cb_read: " << size * nmemb << std::endl;
  OggSoundFile& ogg = *reinterpret_cast<OggSoundFile*>(userdata);

  size_t read_len = size * nmemb;

  // prevent std::istream from hitting eof(), needed as tellg() will
  // return -1 in that case instead of the from cb_tell expected filesize
  read_len = std::min(read_len, static_cast<size_t>(ogg.m_file_size - ogg.m_in.tellg()));

  size_t len = ogg.m_in.read(static_cast<char*>(ptr), read_len).gcount();
  return len;
}

int
OggSoundFile::cb_seek(void* userdata, ogg_int64_t offset, int whence)
{
  OggSoundFile& ogg = *reinterpret_cast<OggSoundFile*>(userdata);

  switch(whence) 
  {
    case SEEK_SET:
      //std::cout << "OggSoundFile::cb_seek: " << offset << " BEG" << std::endl; 
      if (!ogg.m_in.seekg(offset, std::ios::beg))
        return -1;
      break;

    case SEEK_CUR:
      //std::cout << "OggSoundFile::cb_seek: " << offset << " CUR" << std::endl; 
      if (!ogg.m_in.seekg(offset, std::ios::cur))
        return -1;
      break;

    case SEEK_END:
      //std::cout << "OggSoundFile::cb_seek: " << offset << " END" << std::endl; 
      if (!ogg.m_in.seekg(offset, std::ios::end))
        return -1;
      break;

    default:
      return -1;
  }
  return 0;
}
  
int
OggSoundFile::cb_close(void* userdata)
{
  //std::cout << "OggSoundFile::cb_close" << std::endl;
  OggSoundFile& ogg = *reinterpret_cast<OggSoundFile*>(userdata);

  ogg.m_in.close();
  return 0;
}

long
OggSoundFile::cb_tell(void* userdata)
{
  //std::cout << "OggSoundFile::cb_tell" << std::endl;
  OggSoundFile& ogg = *reinterpret_cast<OggSoundFile*>(userdata);
  return static_cast<long>(ogg.m_in.tellg());
}

/* EOF */
