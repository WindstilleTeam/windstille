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

#include <assert.h>
#include <iostream>
#include <assert.h>
#include <stdexcept>
#include <sstream>
#include <boost/scoped_ptr.hpp>

#include "input_manager_sdl.hpp"
#include "input_manager_impl.hpp"
#include "input_manager.hpp"

InputManager::InputManager()
  : impl(new InputManagerSDL())
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

const Controller&
InputManager::get_controller()
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
