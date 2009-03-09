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

#include <SDL.h>
#include <boost/scoped_array.hpp>
#include "stream_sound_source.hpp"
#include "sound_manager.hpp"
#include "sound_file.hpp"

StreamSoundSource::StreamSoundSource(std::auto_ptr<SoundFile> file_)
{
  file = file_;

  alGenBuffers(STREAMFRAGMENTS, buffers);
  SoundManager::check_al_error("Couldn't allocate audio buffers: ");

  format = SoundManager::get_sample_format(file.get());

  try 
    {
      for(size_t i = 0; i < STREAMFRAGMENTS; ++i) 
        {
          fillBufferAndQueue(buffers[i]);
        }
    }
  catch(...) 
    {
      alDeleteBuffers(STREAMFRAGMENTS, buffers);
    }
}

StreamSoundSource::~StreamSoundSource()
{
  alDeleteBuffers(STREAMFRAGMENTS, buffers);
  SoundManager::check_al_error("Couldn't delete audio buffers: ");
}

void
StreamSoundSource::update()
{
  if (playing())
    {
      ALint processed = 0;
      alGetSourcei(source, AL_BUFFERS_PROCESSED, &processed);

      while (processed > 0) 
        {
          processed--;

          ALuint buffer;
          alSourceUnqueueBuffers(source, 1, &buffer);
          SoundManager::check_al_error("Couldn't unqueu audio buffer: ");

          fillBufferAndQueue(buffer);
        }
  
      // we might have to restart the source if we had a buffer underrun
      if (!playing()) {
        std::cerr << "Restarting audio source because of buffer underrun.\n";
        alSourcePlay(source);
        SoundManager::check_al_error("Couldn't restart audio source: ");
      }

      if (fade_state == FadingOn) 
        {
          unsigned int ticks = SDL_GetTicks();
          float time = (ticks - fade_start_ticks) / 1000.0;
          if (time >= fade_time)
            {
              set_gain(1.0);
              fade_state = NoFading;
            } 
          else 
            {
              set_gain(time / fade_time);
            }
        } 
      else if (fade_state == FadingOff) 
        {
          unsigned int ticks = SDL_GetTicks();
          float time = (ticks - fade_start_ticks) / 1000.0;

          if (time >= fade_time) 
            {
              stop();
              fade_state = NoFading;
            } 
          else 
            {
              set_gain( (fade_time-time) / fade_time);
            }
        }
    }
}

void
StreamSoundSource::setFading(FadeState state_, float fade_time_)
{
  fade_state       = state_;
  fade_time        = fade_time_;
  fade_start_ticks = SDL_GetTicks();
}

void
StreamSoundSource::fillBufferAndQueue(ALuint buffer)
{
  // fill buffer
  boost::scoped_array<char> bufferdata(new char[STREAMFRAGMENTSIZE]);
  size_t bytesread = 0;

  do 
    {
      bytesread += file->read(bufferdata.get() + bytesread,
                              STREAMFRAGMENTSIZE - bytesread);
      
      if (bytesread < STREAMFRAGMENTSIZE) 
        {
          file->reset();
        }
    } 
  while(bytesread < STREAMFRAGMENTSIZE);
  
  alBufferData(buffer, format, bufferdata.get(), STREAMFRAGMENTSIZE, file->rate);

  SoundManager::check_al_error("Couldn't refill audio buffer: ");

  alSourceQueueBuffers(source, 1, &buffer);
  SoundManager::check_al_error("Couldn't queue audio buffer: ");
}

/* EOF */
