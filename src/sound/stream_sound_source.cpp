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

#include "sound/stream_sound_source.hpp"

#include <iostream>

#include "sound/sound_manager.hpp"
#include "sound/sound_file.hpp"

StreamSoundSource::StreamSoundSource(SoundChannel& channel, std::auto_ptr<SoundFile> sound_file) :
  SoundSource(channel),
  m_sound_file(sound_file),
  m_format(SoundManager::get_sample_format(m_sound_file.get())),
  m_fade_state(),
  m_fade_start_ticks(),
  m_fade_time(),
  m_total_time(0.0f)
{
  alGenBuffers(STREAMFRAGMENTS, m_buffers);
  SoundManager::check_al_error("Couldn't allocate audio buffers: ");

  try 
  {
    for(size_t i = 0; i < STREAMFRAGMENTS; ++i) 
    {
      fill_buffer_and_queue(m_buffers[i]);
    }
  }
  catch(...) 
  {
    alDeleteBuffers(STREAMFRAGMENTS, m_buffers);
    throw;
  }
}

StreamSoundSource::~StreamSoundSource()
{
  stop();

  alDeleteBuffers(STREAMFRAGMENTS, m_buffers);
  SoundManager::check_al_error("Couldn't delete audio buffers: ");
}

void
StreamSoundSource::seek_to(float sec)
{
  m_sound_file->seek_to(sec);
}

void
StreamSoundSource::update(float delta)
{
  m_total_time += delta;

  if (is_playing())
  {
    ALint processed = 0;
    alGetSourcei(m_source, AL_BUFFERS_PROCESSED, &processed);

    while (processed > 0) 
    {
      processed--;

      ALuint buffer;
      alSourceUnqueueBuffers(m_source, 1, &buffer);
      SoundManager::check_al_error("Couldn't unqueue audio buffer: ");

      fill_buffer_and_queue(buffer);
    }
  
    // we might have to restart the source if we had a buffer underrun
    if (!is_playing()) 
    {
      std::cerr << "Restarting audio source because of buffer underrun.\n";
      alSourcePlay(m_source);
      SoundManager::check_al_error("Couldn't restart audio source: ");
    }

    if (m_fade_state == kFadingOn) 
    {
      float time = m_fade_start_ticks - m_total_time;
      if (time >= m_fade_time)
      {
        set_gain(1.0);
        m_fade_state = kNoFading;
      } 
      else 
      {
        set_gain(time / m_fade_time);
      }
    } 
    else if (m_fade_state == kFadingOff) 
    {
      float time = m_fade_start_ticks - m_total_time;

      if (time >= m_fade_time) 
      {
        stop();
        m_fade_state = kNoFading;
      } 
      else 
      {
        set_gain( (m_fade_time - time) / m_fade_time);
      }
    }
  }
}

void
StreamSoundSource::set_fading(FadeState fade_state, float fade_time)
{
  m_fade_state       = fade_state;
  m_fade_time        = fade_time;
  m_fade_start_ticks = m_total_time;
}

void
StreamSoundSource::fill_buffer_and_queue(ALuint buffer)
{
  char bufferdata[STREAMFRAGMENTSIZE];
  size_t bytesread = 0;

  // fill buffer with data from m_sound_file
  do 
  {
    bytesread += m_sound_file->read(bufferdata + bytesread,
                                    STREAMFRAGMENTSIZE - bytesread);

    // if the end is reached, start from the beginning, thus loop
    if (bytesread < STREAMFRAGMENTSIZE) 
    {
      m_sound_file->reset();
    }
  }
  while(bytesread < STREAMFRAGMENTSIZE);
  
  // upload data to the OpenAL buffer
  alBufferData(buffer, m_format, bufferdata, STREAMFRAGMENTSIZE, m_sound_file->get_rate());
  SoundManager::check_al_error("Couldn't refill audio buffer: ");

  // add buffer to the queue of this source
  alSourceQueueBuffers(m_source, 1, &buffer);
  SoundManager::check_al_error("Couldn't queue audio buffer: ");
}

/* EOF */
