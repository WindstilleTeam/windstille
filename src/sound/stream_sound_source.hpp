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

#ifndef HEADER_WINDSTILLE_SOUND_STREAM_SOUND_SOURCE_HPP
#define HEADER_WINDSTILLE_SOUND_STREAM_SOUND_SOURCE_HPP

#include <memory>
#include <stdio.h>

#include "sound/sound_source.hpp"

class SoundFile;
class SoundChannel;

class StreamSoundSource : public SoundSource
{
public:
  enum FadeState { NoFading, FadingOn, FadingOff };

public:
  StreamSoundSource(SoundChannel& channel, std::auto_ptr<SoundFile> file);
  virtual ~StreamSoundSource();

  void setFading(FadeState state, float fadetime);
  FadeState getFadeState() const { return fade_state; }
  void update(float delta);
  
private:
  void fillBufferAndQueue(ALuint buffer);

private:
  static const size_t STREAMBUFFERSIZE   = 1024 * 500;
  static const size_t STREAMFRAGMENTS    = 5;
  static const size_t STREAMFRAGMENTSIZE = STREAMBUFFERSIZE / STREAMFRAGMENTS;

  std::auto_ptr<SoundFile> file;
  ALuint buffers[STREAMFRAGMENTS];
  ALenum format;

  FadeState fade_state;
  unsigned int fade_start_ticks;
  float fade_time;

  // FIXME: simple time counter that summarizes all deltas, could be done better
  float m_total_time;
};

#endif

/* EOF */
