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

#include <AL/al.h>

#include "math/vector2f.hpp"
#include "sound/sound_source.hpp"

class SoundChannel;

class OpenALSoundSource : public SoundSource
{
public:
  OpenALSoundSource(SoundChannel& channel);
  virtual ~OpenALSoundSource();

  virtual void play();
  virtual void stop();
  virtual bool is_playing() const;

  virtual float get_length() const;

  virtual void set_looping(bool looping);

  /// Set volume (0.0 is silent, 1.0 is normal)
  virtual void  set_gain(float gain);
  virtual float get_gain() const;

  virtual void  seek_to(float sec);

  /** Return the current position in seconds */
  virtual float get_pos() const;

  virtual int get_sample_pos() const;

  virtual void set_position(const Vector2f& position);
  virtual void set_velocity(const Vector2f& position);

  /** Distances closer then reference distance will increase the gain
      (or keep it at 1.0f), while distances further away will lower it */
  virtual void set_reference_distance(float distance);

  /** Higher factor will mean the sound gain lowers quicker with
      distance, while lower factor will reduce the lowering of gain,
      0.0f disables distance based reduction completly */
  virtual void set_rolloff_factor(float factor);
  
  /** Needs to be called whenever the SoundChannels gain changes */
  virtual void update_gain() const;

  virtual void update(float delta) {}

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
