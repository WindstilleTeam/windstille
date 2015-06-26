/*
**  Windstille - A Sci-Fi Action-Adventure Game
**  Copyright (C) 2010 Ingo Ruhnke <grumbel@gmail.com>
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

#include "sound/filtered_sound_file.hpp"

#include <iostream>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>

FilteredSoundFile::FilteredSoundFile(std::unique_ptr<SoundFile> sound_file) :
  m_sound_file(std::move(sound_file))
{
}

FilteredSoundFile::~FilteredSoundFile()
{
}

size_t
FilteredSoundFile::read(void* buffer, size_t buffer_size)
{
  size_t len = m_sound_file->read(buffer, buffer_size);

  // apply filter here
  // FIXME: add a bit of noise as proof of concept
  int16_t* p = static_cast<int16_t*>(buffer);
  for(size_t i = 0; i < len/2; i += 1)
  {
    p[i] = static_cast<int16_t>(p[i] + rand() % 8192 - 4096);
  }

  return len;
}

void
FilteredSoundFile::reset()
{
  return m_sound_file->reset();
}

bool
FilteredSoundFile::eof() const
{
  return m_sound_file->eof();
}

int
FilteredSoundFile::get_bits_per_sample() const
{
  return m_sound_file->get_bits_per_sample();
}

size_t
FilteredSoundFile::get_size() const
{
  return m_sound_file->get_size();
}

int
FilteredSoundFile::get_rate() const
{
  return m_sound_file->get_rate();
}

int
FilteredSoundFile::get_channels() const
{
  return m_sound_file->get_channels();
}

void
FilteredSoundFile::seek_to(float sec)
{
  m_sound_file->seek_to(sec);
}

float
FilteredSoundFile::get_length() const
{
  return m_sound_file->get_length();
}

/* EOF */
