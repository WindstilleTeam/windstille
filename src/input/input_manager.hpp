/*
**  Windstille - A Sci-Fi Action-Adventure Game
**  Copyright (C) 2000,2005 Ingo Ruhnke <grumbel@gmx.de>
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

#ifndef HEADER_INPUT_MANAGER_HXX
#define HEADER_INPUT_MANAGER_HXX

#include <string>
#include <vector>
#include "controller.hpp"
#include "input_event.hpp"

class InputManagerImpl;

/** */
class InputManager
{
private:
  static InputManagerImpl* impl;

public:
  /** Init the InputManager */
  static void init();
  static void deinit();

  /** Load configuration file \a filename */
  static void load(const std::string& filename);

  static void update(float delta);
  static const Controller& get_controller();
  static void clear();

private:
  InputManager(const InputManager&);
  InputManager& operator=(const InputManager&);
};

#endif

/* EOF */
