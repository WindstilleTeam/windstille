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

#ifndef HEADER_WINDSTILLE_APP_APP_HPP
#define HEADER_WINDSTILLE_APP_APP_HPP

namespace wstinput {
class InputManagerSDL;
} // namespace wstinput

namespace sprite3d {
class Manager;
} // namespace sprite3d;

class Fonts;
class OpenGLWindow;
class SoundManager;
class SpriteManager;
class SurfaceManager;
class TextureManager;

class App
{
  friend class WindstilleMain;
public:
  App();

  wstinput::InputManagerSDL& input() const;
  SoundManager& sound() const;
  OpenGLWindow& window() const;
  TextureManager& texture() const;
  SurfaceManager& surface() const;
  SpriteManager& sprite() const;
  sprite3d::Manager& sprite3d() const;
  Fonts& fonts() const;

private:
  wstinput::InputManagerSDL* m_input_manager;
  SoundManager* m_sound_manager;
  OpenGLWindow* m_window;
  TextureManager* m_texture_manager;
  SurfaceManager* m_surface_manager;
  SpriteManager* m_sprite_manager;
  sprite3d::Manager* m_sprite3d_manager;
  Fonts* m_fonts;

private:
  App(const App&) = delete;
  App& operator=(const App&) = delete;
};

extern App g_app;

#endif

/* EOF */
