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

#include <boost/scoped_array.hpp>
#include <assert.h>
#include <stdexcept>
#include <iostream>
#include <sstream>
#include <memory>

#include "sound_file.hpp"
#include "sound_source.hpp"
#include "stream_sound_source.hpp"

#include "sound_manager.hpp"


SoundManager::SoundManager()
  : device(0), 
    context(0), 
    sound_enabled(false), 
    buffers(),
    sources(),
    music_source(),
    next_music_source(),
    music_enabled(true),
    current_music()
{
  try 
    {
      device = alcOpenDevice(0);

      if (!device)
        {
          print_openal_version();
          throw std::runtime_error("Couldn't open audio device.");
        }
      else
        {
          int attributes[] = { 0 };

          context = alcCreateContext(device, attributes);
          check_alc_error("Couldn't create audio context: ");

          alcMakeContextCurrent(context);
          check_alc_error("Couldn't select audio context: ");

          check_al_error("Audio error after init: ");
          sound_enabled = true;
        }
    } 
  catch(std::exception& e) 
    { // disable sound
      device  = 0;
      context = 0;
      sound_enabled = false;

      std::cerr << "Couldn't initialize audio device:" << e.what() << "\n";
      print_openal_version();
      
      std::cout << "Disabling sound\n";
    }
}

SoundManager::~SoundManager()
{
  music_source.reset();
  next_music_source.reset();

  sources.clear();

  for(SoundBuffers::iterator i = buffers.begin(); i != buffers.end(); ++i) 
    {
      ALuint buffer = i->second;
      alDeleteBuffers(1, &buffer);
    }

  if (context != 0) 
    {
      alcDestroyContext(context);
    }

  if (device != 0)
    {
      alcCloseDevice(device);
    }
}

ALuint
SoundManager::load_file_into_buffer(const std::string& filename)
{
  // open sound file
  std::auto_ptr<SoundFile> file(SoundFile::load(filename));
  
  ALenum format = get_sample_format(file.get());
  ALuint buffer;
  alGenBuffers(1, &buffer);
  check_al_error("Couldn't create audio buffer: ");

  boost::scoped_array<char> samples(new char[file->size]);

  try 
    {
      file->read(samples.get(), file->size);

      alBufferData(buffer, format, samples.get(),
                   static_cast<ALsizei> (file->size),
                   static_cast<ALsizei> (file->rate));

      check_al_error("Couldn't fill audio buffer: ");
    }
  catch(...) 
    {
      throw;
    }

  return buffer;
}

std::auto_ptr<SoundSource>
SoundManager::create_sound_source(const std::string& filename)
{
  if (!sound_enabled)
    {
      return std::auto_ptr<SoundSource>();
    }
  else
    {
      ALuint buffer;
  
      // reuse an existing static sound buffer            
      SoundBuffers::iterator i = buffers.find(filename);
      if (i != buffers.end()) 
        {
          buffer = i->second;
        } 
      else 
        {
          buffer = load_file_into_buffer(filename);
          buffers.insert(std::make_pair(filename, buffer));
        }
  
      std::auto_ptr<SoundSource> source(new SoundSource());

      alSourcei(source->source, AL_BUFFER, buffer);

      return source;
    }
}

void
SoundManager::play(const std::string& filename, const Vector2f& pos)
{
  try
    {
      std::auto_ptr<SoundSource> source = create_sound_source(filename);

      if (source.get())
        {
          if (pos == Vector2f(-1, -1)) 
            {
              alSourcef(source->source, AL_ROLLOFF_FACTOR, 0);
            } 
          else
            {
              source->set_position(pos);
            }

          source->play();
          sources.push_back(boost::shared_ptr<SoundSource>(source.release()));
        }
    }
  catch(std::exception& e) 
    {
      std::cout << "Couldn't play sound " << filename << ": " << e.what() << "\n";
    }
}

void
SoundManager::enable_sound(bool enable)
{
  if (device)
    {
      sound_enabled = enable;
    }
}

void
SoundManager::enable_music(bool enable)
{
  if (device)
    {
      music_enabled = enable;

      if (music_enabled) 
        {
          play_music(current_music);
        } 
      else 
        {
          if (music_source.get())
            {
              music_source.reset();
            }
        }
    }
}

void
SoundManager::stop_music(bool fade)
{
  if (fade) 
    {
      if (music_source.get() &&
          music_source->getFadeState() != StreamSoundSource::FadingOff)
        {
          music_source->setFading(StreamSoundSource::FadingOff, .7f);
        }
    } 
  else 
    {
      music_source.reset();
    }
}

void
SoundManager::play_music(const std::string& filename, bool fade)
{
  if (filename != current_music)
    {
      current_music = filename;

      if (music_enabled)
        {
          try 
            {
              std::auto_ptr<StreamSoundSource> newmusic(new StreamSoundSource(SoundFile::load(filename)));

              alSourcef(newmusic->source, AL_ROLLOFF_FACTOR, 0);
 
              if (fade) 
                {
                  if (music_source.get() &&
                      music_source->getFadeState() != StreamSoundSource::FadingOff)
                    {
                      music_source->setFading(StreamSoundSource::FadingOff, .7f);
                    }

                  next_music_source = newmusic;
                } 
              else 
                {
                  music_source = newmusic;
                  music_source->play();
                  next_music_source.reset();
                }
            }
          catch(std::exception& e) 
            {
              std::cerr << "Couldn't play music file '" << filename << "': "
                        << e.what() << "\n";
            }
        }
    }
}

void
SoundManager::set_listener_position(const Vector2f& pos)
{
  alListener3f(AL_POSITION, pos.x, pos.y, 0);
}

void
SoundManager::set_listener_velocity(const Vector2f& vel)
{
  alListener3f(AL_VELOCITY, vel.x, vel.y, 0);
}

void
SoundManager::set_master_volume(float volume)
{
  alListenerf(AL_GAIN, volume);
}

void
SoundManager::set_voice_volume(float volume)
{
  std::cout << "SoundManager::set_voice_volume(" << volume << "): unimplemented" << std::endl;
}

void
SoundManager::set_sfx_volume(float volume)
{
  std::cout << "SoundManager::set_sfx_volume(" << volume << "): unimplemented" << std::endl;
}

void
SoundManager::set_music_volume(float volume)
{
  std::cout << "SoundManager::set_music_volume(" << volume << "): unimplemented" << std::endl;
}

void
SoundManager::update()
{
  if (sound_enabled)
    {
      // check for finished sound sources
      for(SoundSources::iterator i = sources.begin(); i != sources.end(); ) 
        {
          if (!(*i)->playing())
            {
              i = sources.erase(i);
            } 
          else 
            {
              ++i;
            }
        }

      // check streaming sounds
      if (music_source.get()) 
        {
          music_source->update();
        }
  
      if (next_music_source.get() && (!music_source.get() || !music_source->playing())) 
        {
          music_source = next_music_source;
          //music_source->setFading(StreamSoundSource::FadingOn, 1.0f);
          music_source->play();
        }
  
      alcProcessContext(context);
      check_alc_error("Error while processing audio context: ");
    }
}

ALenum
SoundManager::get_sample_format(SoundFile* file)
{
  if (file->channels == 2) 
    {
      if (file->bits_per_sample == 16) 
        {
          return AL_FORMAT_STEREO16;
        } 
      else if (file->bits_per_sample == 8) 
        {
          return AL_FORMAT_STEREO8;
        }
      else 
        {
          throw std::runtime_error("Only 16 and 8 bit samples supported");
        }
    } 
  else if (file->channels == 1) 
    {
      if (file->bits_per_sample == 16) 
        {
          return AL_FORMAT_MONO16;
        }
      else if (file->bits_per_sample == 8) 
        {
          return AL_FORMAT_MONO8;
        }
      else 
        {
          throw std::runtime_error("Only 16 and 8 bit samples supported");
        }
    }
  else 
    {
      throw std::runtime_error("Only 1 and 2 channel samples supported");
    }
}

void
SoundManager::print_openal_version()
{
  std::cout << "OpenAL Vendor: " << alGetString(AL_VENDOR) << "\n"
            << "OpenAL Version: " << alGetString(AL_VERSION) << "\n" 
            << "OpenAL Renderer: " << alGetString(AL_RENDERER) << "\n"
            << "OpenAl Extensions: " << alGetString(AL_RENDERER) << "\n";
}

void
SoundManager::check_alc_error(const char* message)
{
  int err = alcGetError(device);

  if (err != ALC_NO_ERROR) 
    {
      std::stringstream msg;
      msg << message << alcGetString(device, err);
      throw std::runtime_error(msg.str());
    }                
}

void
SoundManager::check_al_error(const char* message)
{
  int err = alGetError();

  if (err != AL_NO_ERROR) 
    {
      std::stringstream msg;
      msg << message << alGetString(err);
      throw std::runtime_error(msg.str());
    }  
}

/* EOF */
