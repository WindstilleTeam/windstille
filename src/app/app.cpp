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

#include "app/app.hpp"

#include <assert.h>

#include <wstsound/sound_manager.hpp>

App g_app;

App::App() :
  m_input_manager(nullptr),
  m_sound_manager(nullptr),
  m_window(nullptr),
  m_texture_manager(nullptr),
  m_surface_manager(nullptr),
  m_sprite_manager(nullptr),
  m_sprite3d_manager(nullptr)
{
}

SoundManager&
App::sound() const
{
  assert(m_sound_manager != nullptr);
  return *m_sound_manager;
}

wstinput::InputManagerSDL&
App::input() const
{
  assert(m_input_manager != nullptr);
  return *m_input_manager;
}

OpenGLWindow&
App::window() const
{
  assert(m_window != nullptr);
  return *m_window;
}

TextureManager&
App::texture() const
{
  assert(m_texture_manager != nullptr);
  return *m_texture_manager;
}

SurfaceManager&
App::surface() const
{
  assert(m_surface_manager != nullptr);
  return *m_surface_manager;
}

SpriteManager&
App::sprite() const
{
  assert(m_sprite_manager != nullptr);
  return *m_sprite_manager;
}

sprite3d::Manager&
App::sprite3d() const
{
  assert(m_sprite3d_manager != nullptr);
  return *m_sprite3d_manager;
}

/* EOF */
