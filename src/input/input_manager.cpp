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

#include "input/input_manager_sdl.hpp"
#include "input/input_manager.hpp"

InputManager::InputManager(const ControllerDescription& controller_description)
  : impl(new InputManagerSDL(controller_description))
{
}

InputManager::~InputManager()
{
}

void
InputManager::load(const std::string& filename)
{
  impl->load(filename);
}

void
InputManager::update(float delta)
{
  assert(impl);
  impl->update(delta);
}

const ControllerDescription&
InputManager::get_controller_description() const
{
  assert(impl);
  return impl->get_controller_description();
}

const Controller&
InputManager::get_controller() const
{
  assert(impl);
  return impl->get_controller();
}

void
InputManager::clear()
{
  impl->clear();
}

/* EOF */
