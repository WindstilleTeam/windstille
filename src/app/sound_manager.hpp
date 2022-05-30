/*
**  Windstille - A Sci-Fi Action-Adventure Game
**  Copyright (C) 2018 Ingo Ruhnke <grumbel@gmail.com>
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

#ifndef HEADER_WINDSTILLE_APP_SOUND_MANAGER_HPP
#define HEADER_WINDSTILLE_APP_SOUND_MANAGER_HPP

#include <memory>

#include <wstsound/fwd.hpp>

namespace wstsound {
class SoundManager;
} // namespace wstsound

class Pathname;

class SoundManager
{
public:
  SoundManager();
  ~SoundManager();

  void play(Pathname const& filename);

  void play_music(Pathname const& filename);
  void stop_music(bool fade = true);

  void enable_sound(bool enable);
  void enable_music(bool enable);

  void set_master_gain(float volume);
  void set_music_gain(float volume);
  void set_sound_gain(float volume);
  void set_voice_gain(float volume);

  void update(float delta);

  wstsound::SoundManager& get_mgr() const { return *m_manager; }

private:
  std::unique_ptr<wstsound::SoundManager> m_manager;

  wstsound::SoundSourcePtr m_current_music;
  wstsound::SoundSourcePtr m_next_music;

private:
  SoundManager(SoundManager const&) = delete;
  SoundManager& operator=(SoundManager const&) = delete;
};

#endif

/* EOF */
