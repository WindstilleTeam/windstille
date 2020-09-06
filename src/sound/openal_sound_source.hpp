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

#ifndef HEADER_WINDSTILLE_SOUND_OPENAL_SOUND_SOURCE_HPP
#define HEADER_WINDSTILLE_SOUND_OPENAL_SOUND_SOURCE_HPP

#ifdef __APPLE__
#  include <OpenAL/al.h>
#else
#  include <AL/al.h>
#endif

#include "math/vector2f.hpp"
#include "sound/sound_source.hpp"

class SoundChannel;

class OpenALSoundSource : public SoundSource
{
public:
  OpenALSoundSource(SoundChannel& channel);
  ~OpenALSoundSource() override;

  void play() override;
  void stop() override;
  bool is_playing() const override;

  float get_length() const override;

  void set_looping(bool looping) override;

  /// Set volume (0.0 is silent, 1.0 is normal)
  void  set_gain(float gain) override;
  float get_gain() const override;

  void  seek_to(float sec) override;

  /** Return the current position in seconds */
  float get_pos() const override;

  /** Seems to be limited to an accuracy of 1024 samples */
  int get_sample_pos() const override;

  void set_position(const Vector2f& position) override;
  void set_velocity(const Vector2f& position) override;

  /** Distances closer then reference distance will increase the gain
      (or keep it at 1.0f), while distances further away will lower it */
  void set_reference_distance(float distance) override;

  /** Higher factor will mean the sound gain lowers quicker with
      distance, while lower factor will reduce the lowering of gain,
      0.0f disables distance based reduction completly */
  void set_rolloff_factor(float factor) override;

  /** Needs to be called whenever the SoundChannels gain changes */
  void update_gain() const override;

  void update(float delta) override {}

protected:
  SoundChannel& m_channel;
  ALuint m_source;
  float m_gain;

private:
  OpenALSoundSource(const OpenALSoundSource&);
  OpenALSoundSource& operator=(const OpenALSoundSource&);
};

#endif

/* EOF */
