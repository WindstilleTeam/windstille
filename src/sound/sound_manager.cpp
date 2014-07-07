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
#include <stdexcept>
#include <iostream>
#include <sstream>

#include "sound/sound_file.hpp"
#include "sound/stream_sound_source.hpp"
#include "sound/static_sound_source.hpp"
#include "sound/sound_manager.hpp"
#include "util/pathname.hpp"


SoundManager::SoundManager() :
  m_device(0), 
  m_context(0), 
  m_sound_enabled(false), 
  m_voice_channel(*this),
  m_sound_channel(*this),
  m_music_channel(*this),
  m_buffers(),
  m_sources(),
  m_music_source(),
  m_next_music_source(),
  m_music_enabled(true),
  m_current_music()
{
  try 
  {
    m_device = alcOpenDevice(NULL);

    if (!m_device)
    {
      print_openal_version();
      throw std::runtime_error("Couldn't open audio device.");
    }
    else
    {
      m_context = alcCreateContext(m_device, NULL);
      check_alc_error("Couldn't create audio context: ");

      alcMakeContextCurrent(m_context);
      check_alc_error("Couldn't select audio context: ");

      check_al_error("Audio error after init: ");
      m_sound_enabled = true;
    }
  } 
  catch(std::exception& e) 
  { // disable sound
    m_device  = 0;
    m_context = 0;
    m_sound_enabled = false;

    std::cerr << "Couldn't initialize audio device:" << e.what() << "\n";
    print_openal_version();
      
    std::cout << "Disabling sound\n";
  }
}

SoundManager::~SoundManager()
{
  m_music_source.reset();
  m_next_music_source.reset();

  m_sources.clear();

  for(SoundBuffers::iterator i = m_buffers.begin(); i != m_buffers.end(); ++i) 
  {
    ALuint buffer = i->second;
    alDeleteBuffers(1, &buffer);
  }

  if (m_context)
  {
    alcDestroyContext(m_context);
  }

  if (m_device)
  {
    alcCloseDevice(m_device);
  }
}

ALuint
SoundManager::load_file_into_buffer(const Pathname& filename)
{
  // open sound file
  std::unique_ptr<SoundFile> file(SoundFile::load(filename));
  
  ALenum format = get_sample_format(file.get());
  ALuint buffer;
  alGenBuffers(1, &buffer);
  check_al_error("Couldn't create audio buffer: ");

  try 
  {
    boost::scoped_array<char> samples(new char[file->get_size()]);
    
    file->read(samples.get(), file->get_size());

    alBufferData(buffer, format, samples.get(),
                 static_cast<ALsizei> (file->get_size()),
                 static_cast<ALsizei> (file->get_rate()));

    check_al_error("Couldn't fill audio buffer: ");
  }
  catch(...) 
  {
    throw;
  }

  return buffer;
}

SoundSourcePtr
SoundManager::create_sound_source(const Pathname& filename, SoundChannel& channel, OpenALSoundSourceType type)
{
  if (!m_sound_enabled)
  {
    return SoundSourcePtr();
  }
  else
  { 
    switch(type)
    {
      case kStaticSoundSource:
      {
        ALuint buffer;
  
        // reuse an existing static sound buffer            
        SoundBuffers::iterator i = m_buffers.find(filename);
        if (i != m_buffers.end()) 
        {
          buffer = i->second;
        } 
        else 
        {
          buffer = load_file_into_buffer(filename);
          m_buffers.insert(std::make_pair(filename, buffer));
        }

        return SoundSourcePtr(new StaticSoundSource(channel, buffer));
      }
      break;

      case kStreamSoundSource:
      {
        std::unique_ptr<SoundFile> sound_file = SoundFile::load(filename);
        return SoundSourcePtr(new StreamSoundSource(channel, std::move(sound_file)));
      }
      break;

      default:
        assert(!"never reached");
        return SoundSourcePtr();
    }
  }
}

SoundSourcePtr
SoundManager::play(const Pathname& filename, const Vector2f& pos)
{
  try
  {
    SoundSourcePtr source = create_sound_source(filename, m_sound_channel, kStaticSoundSource);

    if (source.get())
    {
      source->set_position(pos);
      source->play();
      m_sources.push_back(source);
    }

    return source;
  }
  catch(std::exception& e) 
  {
    std::cout << "Couldn't play sound " << filename << ": " << e.what() << "\n";
    return SoundSourcePtr(); // FIXME: not the best idea, should return dummy object
  }
}

void
SoundManager::enable_sound(bool enable)
{
  if (m_device)
  {
    m_sound_enabled = enable;
  }
}

void
SoundManager::enable_music(bool enable)
{
  if (m_device)
  {
    m_music_enabled = enable;

    if (m_music_enabled) 
    {
      play_music(m_current_music);
    } 
    else 
    {
      if (m_music_source.get())
      {
        m_music_source.reset();
      }
    }
  }
}

void
SoundManager::stop_music(bool fade)
{
  if (fade) 
  {
    if (m_music_source.get() &&
        m_music_source->get_fade_state() != StreamSoundSource::kFadingOff)
    {
      m_music_source->set_fading(StreamSoundSource::kFadingOff, .7f);
    }
  } 
  else 
  {
    m_music_source.reset();
  }
}

void
SoundManager::play_music(const Pathname& filename, bool fade)
{
  if (filename != m_current_music)
  {
    m_current_music = filename;

    if (m_music_enabled)
    {
      try 
      {
        std::unique_ptr<StreamSoundSource> newmusic(new StreamSoundSource(m_music_channel, SoundFile::load(filename)));

        if (fade) 
        {
          if (m_music_source.get() &&
              m_music_source->get_fade_state() != StreamSoundSource::kFadingOff)
          {
            m_music_source->set_fading(StreamSoundSource::kFadingOff, .7f);
          }

          m_next_music_source = std::move(newmusic);
        }
        else
        {
          m_music_source = std::move(newmusic);
          m_music_source->play();
          m_next_music_source.reset();
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
SoundManager::set_gain(float gain)
{
  alListenerf(AL_GAIN, gain);
}

void
SoundManager::update(float delta)
{
  m_voice_channel.update(delta);
  m_sound_channel.update(delta);
  m_music_channel.update(delta);

  if (m_sound_enabled)
  {
    // check for finished sound sources
    for(SoundSources::iterator i = m_sources.begin(); i != m_sources.end(); ) 
    {
      if (!(*i)->is_playing())
      {
        i = m_sources.erase(i);
      } 
      else 
      {
        ++i;
      }
    }

    // check streaming sounds
    if (m_music_source.get()) 
    {
      m_music_source->update(delta);
    }
  
    if (m_next_music_source.get() && (!m_music_source.get() || !m_music_source->is_playing())) 
    {
      m_music_source = std::move(m_next_music_source);
      //music_source->setFading(StreamSoundSource::FadingOn, 1.0f);
      m_music_source->play();
    }
  
    alcProcessContext(m_context);
    check_alc_error("Error while processing audio context: ");
  }
}

ALenum
SoundManager::get_sample_format(SoundFile* file)
{
  if (file->get_channels() == 2) 
  {
    if (file->get_bits_per_sample() == 16) 
    {
      return AL_FORMAT_STEREO16;
    } 
    else if (file->get_bits_per_sample() == 8) 
    {
      return AL_FORMAT_STEREO8;
    }
    else 
    {
      throw std::runtime_error("Only 16 and 8 bit samples supported");
    }
  } 
  else if (file->get_channels() == 1) 
  {
    if (file->get_bits_per_sample() == 16) 
    {
      return AL_FORMAT_MONO16;
    }
    else if (file->get_bits_per_sample() == 8) 
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
            << "OpenAL Extensions: " << alGetString(AL_RENDERER) << "\n";
}

void
SoundManager::check_alc_error(const char* message)
{
  int err = alcGetError(m_device);

  if (err != ALC_NO_ERROR) 
  {
    std::stringstream msg;
    msg << message << alcGetString(m_device, err);
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
