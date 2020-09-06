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

#ifndef HEADER_WINDSTILLE_SOUND_WAV_SOUND_FILE_HPP
#define HEADER_WINDSTILLE_SOUND_WAV_SOUND_FILE_HPP

#include <fstream>

#include "sound/sound_file.hpp"

class WavSoundFile : public SoundFile
{
public:
  WavSoundFile(const Pathname& filename);
  ~WavSoundFile() override;

  bool eof() const override;
  size_t read(void* buffer, size_t buffer_size) override;
  void reset() override;
  void seek_to(float sec) override;

private:
  std::ifstream file;
  bool m_eof;
  size_t datastart;
  int m_channels;
  int m_rate;
  int m_bits_per_sample;
  size_t m_size; /// size in bytes

public:
  int    get_bits_per_sample() const override { return m_bits_per_sample; }
  size_t get_size() const override { return m_size; }
  int    get_rate() const override { return m_rate; }
  int    get_channels() const override { return m_channels; }

private:
  WavSoundFile(const WavSoundFile&);
  WavSoundFile& operator=(const WavSoundFile&);
};

#endif

/* EOF */
