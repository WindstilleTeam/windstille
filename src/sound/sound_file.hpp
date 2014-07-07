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

#ifndef HEADER_WINDSTILLE_SOUND_SOUND_FILE_HPP
#define HEADER_WINDSTILLE_SOUND_SOUND_FILE_HPP

#include <memory>
#include <string>

class Pathname;

class SoundFile
{
public:
  SoundFile() {}
  virtual ~SoundFile() {}

  virtual size_t read(void* buffer, size_t buffer_size) = 0;
  virtual void reset() = 0;
  virtual bool eof() const = 0;

  virtual int    get_bits_per_sample() const = 0;
  virtual size_t get_size() const = 0;
  virtual int    get_rate() const = 0;
  virtual int    get_channels() const = 0;

  virtual void   seek_to(float sec) = 0;
  
  /** Returns the length of the file in seconds */
  virtual float  get_length() const;

public:
  static std::unique_ptr<SoundFile> load(const Pathname& filename);
};

#endif

/* EOF */
