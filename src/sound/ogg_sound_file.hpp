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

#ifndef HEADER_WINDSTILLE_SOUND_OGG_SOUND_FILE_HPP
#define HEADER_WINDSTILLE_SOUND_OGG_SOUND_FILE_HPP

#include <vorbis/vorbisfile.h>
#include <fstream>

#include "sound/sound_file.hpp"

class OggSoundFile : public SoundFile
{
public:
  OggSoundFile(const Pathname& filename);
  ~OggSoundFile();

  size_t read(void* buffer, size_t buffer_size);
  void reset();
  bool eof() const;

  int    get_bits_per_sample() const { return m_bits_per_sample; }
  size_t get_size() const { return m_size; }
  int    get_rate() const { return m_rate; }
  int    get_channels() const { return m_channels; }

  void seek_to(float sec);
  
private:
  // I/O functions for ogg
  static size_t cb_read(void* ptr, size_t size, size_t nmemb, void* source);
  static int    cb_seek(void* source, ogg_int64_t offset, int whence);
  static int    cb_close(void* source);
  static long   cb_tell(void* source);
  
  std::ifstream  m_in;
  bool m_eof;
  size_t m_file_size;
  OggVorbis_File m_vorbis_file;

  int m_channels;
  int m_rate;
  int m_bits_per_sample;
  size_t m_size; /// size in bytes

private:
  OggSoundFile(const OggSoundFile&);
  OggSoundFile& operator=(const OggSoundFile&);
};

#endif

/* EOF */
