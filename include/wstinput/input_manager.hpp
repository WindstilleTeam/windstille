// Windstille Input Library
// Copyright (C) 2005-2020 Ingo Ruhnke <grumbel@gmail.com>
//
// This program is free software: you can redistribute it and/or modify it
// under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation, either version 3 of the License, or (at your
// option) any later version.
//
// This program is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
// or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public
// License for more details.
//
// You should have received a copy of the GNU Lesser General Public License
// along with this program. If not, see <http://www.gnu.org/licenses/>.

#ifndef HEADER_WINDSTILLE_INPUT_INPUT_MANAGER_HPP
#define HEADER_WINDSTILLE_INPUT_INPUT_MANAGER_HPP

#include <SDL.h>
#include <filesystem>
#include <memory>

#include <prio/fwd.hpp>

#include "controller.hpp"
#include "controller_description.hpp"
#include "input_bindings.hpp"

namespace wstinput {

class InputManagerSDLImpl;

class InputManagerSDL
{
public:
  InputManagerSDL(ControllerDescription const& controller_description);
  ~InputManagerSDL();

  void load(std::filesystem::path const& filename);

  void update(float delta);

  void clear();

  std::string keyid_to_string(SDL_Scancode id) const;
  SDL_Scancode string_to_keyid(const std::string& str) const;

  ControllerDescription const& get_controller_description() const { return m_controller_description; }
  Controller const& get_controller() const { return m_controller; }

  InputBindings& bindings() { return m_bindings; }

  void on_event(const SDL_Event& event);

  /** Ensure that the joystick device \a device is open */
  void ensure_open_joystick(int device);

  void start_text_input();
  void stop_text_input();
  bool is_text_input_active() const;

private:
  ControllerDescription m_controller_description;
  Controller m_controller;
  InputBindings m_bindings;
  std::vector<SDL_Joystick*> m_joysticks;
  std::map<std::string, SDL_Scancode> m_keyidmapping;

private:
  InputManagerSDL (const InputManagerSDL&);
  InputManagerSDL& operator= (const InputManagerSDL&);
};

} // namespace wstinput

#endif

/* EOF */
