/*
**  Windstille - A Sci-Fi Action-Adventure Game
**  Copyright (C) 2020 Ingo Ruhnke <grumbel@gmail.com>
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

#include "editor/app.hpp"

#include <assert.h>

App g_app;

App::App() :
  m_texture_manager(nullptr),
  m_surface_manager(nullptr),
  m_sprite_manager(nullptr),
  m_sprite3d_manager(nullptr)
{
}

wstdisplay::TextureManager&
App::texture() const
{
  assert(m_texture_manager != nullptr);
  return *m_texture_manager;
}

wstdisplay::SurfaceManager&
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
