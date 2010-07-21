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

#include "sound/openal_sound_source.hpp"

class SoundFile;
class SoundChannel;

class StreamSoundSource : public OpenALSoundSource
{
public:
  enum FadeState { kNoFading, kFadingOn, kFadingOff };

public:
  StreamSoundSource(SoundChannel& channel, std::auto_ptr<SoundFile> sound_file);
  virtual ~StreamSoundSource();

  void set_fading(FadeState state, float fadetime);
  FadeState get_fade_state() const { return m_fade_state; }

  void update(float delta);
  
  virtual void seek_to(float sec);

private:
  void fill_buffer_and_queue(ALuint buffer);

private:
  static const size_t STREAMFRAGMENTS    = 4;
  static const size_t STREAMFRAGMENTSIZE = 65536;

  std::auto_ptr<SoundFile> m_sound_file;
  ALuint m_buffers[STREAMFRAGMENTS];
  ALenum m_format;

  FadeState m_fade_state;
  float m_fade_start_ticks;
  float m_fade_time;

  // FIXME: simple time counter that summarizes all deltas, could be done better
  float m_total_time;
};

#endif

/* EOF */
