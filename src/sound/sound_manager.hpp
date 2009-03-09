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
#include <string>
#include <vector>
#include <map>

#include <AL/alc.h>
#include <AL/al.h>

#include "math/vector2f.hpp"

typedef void* SoundHandle;

class SoundFile;
class SoundSource;
class StreamSoundSource;

class SoundManager
{
public:
  SoundManager();
  virtual ~SoundManager();

  void enable_sound(bool sound_enabled);
  void enable_music(bool music_enabled);

  void set_listener_position(const Vector2f& position);
  void set_listener_velocity(const Vector2f& velocity);

  void set_master_volume(float volume);
  void set_voice_volume(float volume);
  void set_sfx_volume(float volume);
  void set_music_volume(float volume);

  /**
   * Convenience function to simply play a sound at a given position.
   */
  void play(const std::string& name, const Vector2f& pos = Vector2f(-1, -1));

  void play_music(const std::string& filename, bool fade = true);
  void stop_music(bool fade = true);

  void update();

private:
  friend class SoundSource;
  friend class StreamSoundSource;

  /**
   * Creates a new sound source object which plays the specified soundfile.
   * You are responsible for deleting the sound source later (this will stop the
   * sound).
   * This function might throw exceptions. It returns 0 if no audio device is
   * available.
   */
  std::auto_ptr<SoundSource> create_sound_source(const std::string& filename);

  static ALuint load_file_into_buffer(const std::string& filename);
  static ALenum get_sample_format(SoundFile* file);

  void print_openal_version();
  void check_alc_error(const char* message);
  static void check_al_error(const char* message);

  ALCdevice*  device;
  ALCcontext* context;
  bool sound_enabled;

  typedef std::map<std::string, ALuint> SoundBuffers;
  SoundBuffers buffers;
  typedef std::vector<SoundSource*> SoundSources;
  SoundSources sources;

  std::auto_ptr<StreamSoundSource> music_source;
  std::auto_ptr<StreamSoundSource> next_music_source;

  bool music_enabled;
  std::string current_music;

private:
  static SoundManager* current_; 
public:
  static SoundManager* current() { return current_; } 
};

#endif

/* EOF */
