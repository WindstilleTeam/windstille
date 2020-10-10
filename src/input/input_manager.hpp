/*
**  Windstille - A Sci-Fi Action-Adventure Game
**  Copyright (C) 2005,2007 Ingo Ruhnke <grumbel@gmail.com>
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

#ifndef HEADER_WINDSTILLE_INPUT_INPUT_MANAGER_HPP
#define HEADER_WINDSTILLE_INPUT_INPUT_MANAGER_HPP

#include <SDL.h>
#include <memory>

#include "input/controller.hpp"
#include "input/controller_description.hpp"
#include "util/currenton.hpp"
#include "util/file_reader.hpp"

class InputManagerSDLImpl;
class Pathname;

struct JoystickButtonBinding
{
  int event;
  int device;
  int button;
};

struct JoystickAxisBinding
{
  int  event;
  int  device;
  int  axis;
  bool invert;
};

struct JoystickButtonAxisBinding
{
  int event;
  int device;
  int minus;
  int plus;
};

struct JoystickAxisButtonBinding
{
  int  event;
  int  device;
  int  axis;
  bool up;
};

struct MouseButtonBinding
{
  int event;
  int device;
  int button;
};

struct KeyboardButtonBinding
{
  int event;
  SDL_Scancode key;
};

struct KeyboardAxisBinding
{
  int    event;
  SDL_Scancode minus;
  SDL_Scancode plus;
};

struct WiimoteButtonBinding
{
  int event;
  int device;
  int button;
};

struct WiimoteAxisBinding
{
  int event;
  int device;
  int axis;
};

/** */
class InputManagerSDL : public Currenton<InputManagerSDL>
{
public:
  InputManagerSDL(const ControllerDescription& controller_description);
  ~InputManagerSDL() override;

  void load(const Pathname& filename);

  void update(float delta);

  void bind_joystick_hat_axis(int event, int device, int axis);

  void bind_joystick_axis(int event, int device, int axis, bool invert);
  void bind_joystick_button_axis(int event, int device, int minus, int plus);
  void bind_joystick_button(int event, int device, int button);
  void bind_joystick_axis_button(int event, int device, int axis, bool up);

  void bind_keyboard_button(int event, SDL_Scancode key);
  void bind_keyboard_axis(int event, SDL_Scancode minus, SDL_Scancode plus);

  void bind_mouse_button(int event, int device, int button);

  void bind_wiimote_button(int event, int device, int button);
  void bind_wiimote_axis(int event, int device, int axis);

  void clear_bindings();

  void clear();

  std::string keyid_to_string(SDL_Scancode id) const;
  SDL_Scancode string_to_keyid(const std::string& str) const;

  void on_event(const SDL_Event& event);

  void add_axis_event  (int name, float pos);
  void add_ball_event  (int name, float pos);
  void add_button_event(int name, bool down);
  void add_keyboard_event(int name, KeyboardEvent::KeyType key_type, int code);

  const ControllerDescription& get_controller_description() const { return m_controller_description; }
  const Controller& get_controller() const { return m_controller; }

private:
  void on_key_event(const SDL_KeyboardEvent& key);
  void on_mouse_button_event(const SDL_MouseButtonEvent& button);
  void on_joy_button_event(const SDL_JoyButtonEvent& button);
  void on_joy_axis_event(const SDL_JoyAxisEvent& button);

  /** Ensure that the joystick device \a device is open */
  void ensure_open_joystick(int device);

  void parse_config(ReaderMapping const& reader);

private:
  ControllerDescription m_controller_description;
  Controller m_controller;

  std::vector<JoystickButtonBinding>     m_joystick_button_bindings;
  std::vector<JoystickButtonAxisBinding> m_joystick_button_axis_bindings;
  std::vector<JoystickAxisBinding>       m_joystick_axis_bindings;
  std::vector<JoystickAxisButtonBinding> m_joystick_axis_button_bindings;

  std::vector<KeyboardButtonBinding> m_keyboard_button_bindings;
  std::vector<KeyboardAxisBinding>   m_keyboard_axis_bindings;

  std::vector<MouseButtonBinding>   m_mouse_button_bindings;

  std::vector<WiimoteButtonBinding> m_wiimote_button_bindings;
  std::vector<WiimoteAxisBinding>   m_wiimote_axis_bindings;

  std::vector<SDL_Joystick*> m_joysticks;

  std::map<std::string, SDL_Scancode> m_keyidmapping;

private:
  InputManagerSDL (const InputManagerSDL&);
  InputManagerSDL& operator= (const InputManagerSDL&);
};

#endif

/* EOF */
