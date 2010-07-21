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

#include "sound/sound_source.hpp"

#include <assert.h>
#include <iostream>

#include "sound/sound_manager.hpp"

SoundSource::SoundSource(SoundChannel& channel) :
  m_channel(channel),
  m_source(),
  m_gain(1.0f)
{
  alGenSources(1, &m_source);
  SoundManager::check_al_error("Couldn't create audio source: ");
  set_reference_distance(128);
}

SoundSource::~SoundSource()
{
  stop();
  alDeleteSources(1, &m_source);
  SoundManager::check_al_error("Couldn't delete source: ");
}

void
SoundSource::stop()
{
  alSourceStop(m_source);
  alSourcei(m_source, AL_BUFFER, AL_NONE);
  SoundManager::check_al_error("Problem stopping audio source: ");
}

void
SoundSource::play()
{
  alSourcePlay(m_source);
  SoundManager::check_al_error("Couldn't start audio source: ");
}

float
SoundSource::get_length() const
{
  assert(!"implement me");
  return 0.0f;
}

void
SoundSource::seek_to(float sec)
{
  // FIXME: Need to be adopted for streaming sound
  alSourcef(m_source, AL_SEC_OFFSET, sec);
  SoundManager::check_al_error("SoundSource::seek_to: ");
}

float
SoundSource::get_pos() const
{
  float sec = 0.0f;
  alGetSourcef(m_source, AL_SEC_OFFSET, &sec);
  SoundManager::check_al_error("SoundSource::get_pos: ");
  return sec;
}

bool
SoundSource::is_playing() const
{
  ALint state = AL_PLAYING;
  alGetSourcei(m_source, AL_SOURCE_STATE, &state);
  return state != AL_STOPPED;
}

void
SoundSource::set_looping(bool looping)
{
  alSourcei(m_source, AL_LOOPING, looping ? AL_TRUE : AL_FALSE);
  SoundManager::check_al_error("SoundSource::set_looping: ");
}

void
SoundSource::set_position(const Vector2f& position)
{
  alSource3f(m_source, AL_POSITION, position.x, position.y, 0);
  SoundManager::check_al_error("SoundSource::set_position: ");
}

void
SoundSource::set_velocity(const Vector2f& velocity)
{
  alSource3f(m_source, AL_VELOCITY, velocity.x, velocity.y, 0);
  SoundManager::check_al_error("SoundSource::set_velocity: ");
}

void
SoundSource::set_gain(float gain)
{
  m_gain = gain;
  alSourcef(m_source, AL_GAIN, m_channel.get_gain() * m_gain);
  SoundManager::check_al_error("SoundSource::set_gain: ");
}

float
SoundSource::get_gain() const
{
  return m_gain;
}

void
SoundSource::set_reference_distance(float distance)
{
  alSourcef(m_source, AL_REFERENCE_DISTANCE, distance);
  SoundManager::check_al_error("SoundSource::set_reference_distance: ");
}

void
SoundSource::set_rolloff_factor(float factor)
{
  alSourcef(m_source, AL_ROLLOFF_FACTOR, factor);
  SoundManager::check_al_error("SoundSource::set_rolloff_factor: ");
}

void
SoundSource::update_gain() const
{
  alSourcef(m_source, AL_GAIN, m_channel.get_gain() * get_gain());
  SoundManager::check_al_error("SoundSource::update_gain: ");
}

void
SoundSource::set_buffer(ALuint buffer)
{
  alSourcei(m_source, AL_BUFFER, buffer);
  SoundManager::check_al_error("SoundSource::set_buffer: ");
}

/* EOF */
