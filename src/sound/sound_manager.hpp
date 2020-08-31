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

#ifndef HEADER_WINDSTILLE_SOUND_SOUND_MANAGER_HPP
#define HEADER_WINDSTILLE_SOUND_SOUND_MANAGER_HPP

#include <memory>
#include <memory>
#include <string>
#include <vector>
#include <map>

#ifdef __APPLE__
#  include <OpenAL/alc.h>
#  include <OpenAL/al.h>
#else
#  include <AL/alc.h>
#  include <AL/al.h>
#endif

#include "math/vector2f.hpp"
#include "sound/sound_channel.hpp"
#include "util/currenton.hpp"
#include "util/pathname.hpp"

typedef void* SoundHandle;

class Pathname;
class SoundFile;
class SoundSource;
class StreamSoundSource;

class SoundManager : public Currenton<SoundManager>
{
public:
  SoundManager();
  virtual ~SoundManager();

  void enable_sound(bool sound_enabled);
  void enable_music(bool music_enabled);

  void set_listener_position(const Vector2f& position);
  void set_listener_velocity(const Vector2f& velocity);

  // master volume is not clamped
  void set_gain(float gain);

  SoundChannel& voice() { return m_voice_channel; }
  SoundChannel& sound() { return m_sound_channel; }
  SoundChannel& music() { return m_music_channel; }

  /**
   * Convenience function to simply play a sound at a given position.
   */
  SoundSourcePtr play(const Pathname& name, const Vector2f& pos = Vector2f(-1, -1));

  void play_music(const Pathname& filename, bool fade = true);
  void stop_music(bool fade = true);

  void update(float delta);

  /**
   * Creates a new sound source object which plays the specified soundfile.
   * You are responsible for deleting the sound source later (this will stop the
   * sound).
   * This function might throw exceptions. It returns 0 if no audio device is
   * available.
   */
  SoundSourcePtr create_sound_source(const Pathname& filename,
                                     SoundChannel& channel,
                                     OpenALSoundSourceType type);

public:
  static void check_al_error(const char* message);
  static ALenum get_sample_format(SoundFile* file);

private:
  static ALuint load_file_into_buffer(const Pathname& filename);

  void print_openal_version();
  void check_alc_error(const char* message);

  ALCdevice*  m_device;
  ALCcontext* m_context;
  bool m_sound_enabled;

  SoundChannel m_voice_channel;
  SoundChannel m_sound_channel;
  SoundChannel m_music_channel;

  typedef std::map<Pathname, ALuint> SoundBuffers;
  SoundBuffers m_buffers;
  typedef std::vector<SoundSourcePtr> SoundSources;
  SoundSources m_sources;

  std::unique_ptr<StreamSoundSource> m_music_source;
  std::unique_ptr<StreamSoundSource> m_next_music_source;

  bool m_music_enabled;
  Pathname m_current_music;

private:
  SoundManager(const SoundManager&);
  SoundManager& operator=(const SoundManager&);
};

#endif

/* EOF */
