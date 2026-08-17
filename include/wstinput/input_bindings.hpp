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

#ifndef HEADER_WINDSTILLE_INPUT_INPUT_BINDINGS_HPP
#define HEADER_WINDSTILLE_INPUT_INPUT_BINDINGS_HPP

#include <filesystem>
#include <vector>

#include <SDL.h>

namespace wstinput {

class Controller;
class ControllerDescription;
class InputManagerSDL;

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

struct MouseMotionBinding
{
  int event;
  int device;
  int axis;
};

struct MouseMotionBallBinding
{
  int event;
  int device;
  int axis;
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


class InputBindings
{
public:
  InputBindings(InputManagerSDL& manager);

  void load(std::filesystem::path const& filename,
            ControllerDescription const& controller_description);

  void bind_joystick_hat_axis(int event, int device, int axis);

  void bind_joystick_axis(int event, int device, int axis, bool invert);
  void bind_joystick_button_axis(int event, int device, int minus, int plus);
  void bind_joystick_button(int event, int device, int button);
  void bind_joystick_axis_button(int event, int device, int axis, bool up);

  void bind_keyboard_button(int event, SDL_Scancode key);
  void bind_keyboard_axis(int event, SDL_Scancode minus, SDL_Scancode plus);

  void bind_mouse_button(int event, int device, int button);
  void bind_mouse_motion(int event, int device, int axis);
  void bind_mouse_motion_ball(int event, int device, int axis);
  void bind_mouse_wheel(int event, int device, int wheel);

  void bind_wiimote_button(int event, int device, int button);
  void bind_wiimote_axis(int event, int device, int axis);

  void clear();

  void dispatch_event(SDL_Event const& event, Controller& controller) const;

private:
  void dispatch_key_event(SDL_KeyboardEvent const& key, Controller& controller) const;
  void dispatch_mouse_button_event(SDL_MouseButtonEvent const& button, Controller& controller) const;
  void dispatch_mouse_motion_event(SDL_MouseMotionEvent const& motion, Controller& controller) const;
  void dispatch_mouse_wheel_event(SDL_MouseWheelEvent const& wheel, Controller& controller) const;
  void dispatch_joy_button_event(SDL_JoyButtonEvent const& button, Controller& controller) const;
  void dispatch_joy_axis_event(SDL_JoyAxisEvent const& button, Controller& controller) const;

private:
  InputManagerSDL& m_manager;

  std::vector<JoystickButtonBinding>     m_joystick_button_bindings;
  std::vector<JoystickButtonAxisBinding> m_joystick_button_axis_bindings;
  std::vector<JoystickAxisBinding>       m_joystick_axis_bindings;
  std::vector<JoystickAxisButtonBinding> m_joystick_axis_button_bindings;

  std::vector<KeyboardButtonBinding> m_keyboard_button_bindings;
  std::vector<KeyboardAxisBinding>   m_keyboard_axis_bindings;

  std::vector<MouseButtonBinding>   m_mouse_button_bindings;
  std::vector<MouseMotionBinding>   m_mouse_motion_bindings;
  std::vector<MouseMotionBallBinding>   m_mouse_motion_ball_bindings;

  std::vector<WiimoteButtonBinding> m_wiimote_button_bindings;
  std::vector<WiimoteAxisBinding>   m_wiimote_axis_bindings;

private:
  InputBindings(const InputBindings&) = delete;
  InputBindings& operator=(const InputBindings&) = delete;
};

} // namespace wstinput

#endif

/* EOF */
