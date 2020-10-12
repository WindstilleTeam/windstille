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

class SoundManager;
namespace wstinput {
class InputManagerSDL;
} // namespace wstinput

class App
{
  friend class WindstilleMain;
public:
  App();

  SoundManager& sound() const;
  wstinput::InputManagerSDL& input() const;

private:
  SoundManager* m_sound_manager;
  wstinput::InputManagerSDL* m_input_manager;

private:
  App(const App&) = delete;
  App& operator=(const App&) = delete;
};

extern App g_app;

#endif

/* EOF */
