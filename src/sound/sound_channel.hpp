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

#ifndef HEADER_WINDSTILLE_SOUND_SOUND_CHANNEL_HPP
#define HEADER_WINDSTILLE_SOUND_SOUND_CHANNEL_HPP

#include <vector>

#include "sound/sound_source_ptr.hpp"

class Pathname;
class SoundFile;
class SoundManager;

class SoundChannel
{
public:
  SoundChannel(SoundManager& sound_manager);

  // shortcut for prepare()->play()
  SoundSourcePtr play(const Pathname& filename);
  
  SoundSourcePtr prepare(const Pathname& filename);

  void update(float delta);

  // volume is clamped to [0,1]
  void  set_volume(float volume);
  float get_volume() const;

private:
  SoundManager& m_sound_manager;
  std::vector<SoundSourcePtr> m_sound_sources;
  float m_volume;

private:
  SoundChannel(const SoundChannel&);
  SoundChannel& operator=(const SoundChannel&);
};

#endif

/* EOF */
