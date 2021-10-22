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

#include "app/sound_manager.hpp"

#include <wstsound/sound_manager.hpp>

#include "util/pathname.hpp"

SoundManager::SoundManager() :
  m_manager(std::make_unique<wstsound::SoundManager>()),
  m_current_music(),
  m_next_music()
{
}

SoundManager::~SoundManager()
{
}

void
SoundManager::play(Pathname const& filename)
{
  auto source = m_manager->sound().play(filename.get_sys_path());
  m_manager->manage(std::move(source));
}

void
SoundManager::play_music(Pathname const& filename)
{
  m_current_music = m_manager->music().play(filename.get_sys_path());
}

void
SoundManager::stop_music(bool fade)
{
  //m_current_music()->set_fading()
}

void
SoundManager::set_master_gain(float volume)
{
  m_manager->set_gain(volume);
}

void
SoundManager::set_music_gain(float volume)
{
  m_manager->music().set_gain(volume);
}

void
SoundManager::set_sound_gain(float volume)
{
  m_manager->sound().set_gain(volume);
}

void
SoundManager::set_voice_gain(float volume)
{
   m_manager->voice().set_gain(volume);
}

void
SoundManager::enable_sound(bool enable)
{
}

void
SoundManager::enable_music(bool enable)
{
}

void
SoundManager::update(float delta)
{
  m_manager->update(delta);
}

/* EOF */
