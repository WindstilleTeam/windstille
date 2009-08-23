/*
**  Windstille - A Sci-Fi Action-Adventure Game
**  Copyright (C) 2005,2007 Ingo Ruhnke <grumbel@gmx.de>
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

#ifndef HEADER_WINDSTILLE_INPUT_INPUT_MANAGER_SDL_HPP
#define HEADER_WINDSTILLE_INPUT_INPUT_MANAGER_SDL_HPP

#include <SDL.h>
#include <boost/scoped_ptr.hpp>

#include "input/input_manager_impl.hpp"
#include "util/currenton.hpp"

class FileReader;
class InputManagerSDLImpl;

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
  SDLKey key;
};

struct KeyboardAxisBinding
{
  int    event;
  SDLKey minus;
  SDLKey plus;
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
class InputManagerSDL : public InputManagerImpl,
                        public Currenton<InputManagerSDL>
{
public:
  InputManagerSDL();
  virtual ~InputManagerSDL();

  void load(const std::string& filename);

  void update(float delta);

  void bind_joystick_hat_axis(int event, int device, int axis);

  void bind_joystick_axis(int event, int device, int axis, bool invert);
  void bind_joystick_button_axis(int event, int device, int minus, int plus);
  void bind_joystick_button(int event, int device, int button);
  void bind_joystick_axis_button(int event, int device, int axis, bool up);

  void bind_keyboard_button(int event, SDLKey key);
  void bind_keyboard_axis(int event, SDLKey minus, SDLKey plus);

  void bind_mouse_button(int event, int device, int button);

  void bind_wiimote_button(int event, int device, int button);
  void bind_wiimote_axis(int event, int device, int axis);
  
  void clear_bindings();

  std::string keyid_to_string(SDLKey id);
  SDLKey      string_to_keyid(const std::string& str);

  void on_event(const SDL_Event& event);

  void add_axis_event  (int name, float pos);

private:
  void on_key_event(const SDL_KeyboardEvent& key);
  void on_mouse_button_event(const SDL_MouseButtonEvent& button);
  void on_joy_button_event(const SDL_JoyButtonEvent& button);
  void on_joy_axis_event(const SDL_JoyAxisEvent& button);
  
  /** Ensure that the joystick device \a device is open */
  void ensure_open_joystick(int device);

  void parse_config(FileReader& reader);

  boost::scoped_ptr<InputManagerSDLImpl> impl;

private:
  InputManagerSDL (const InputManagerSDL&);
  InputManagerSDL& operator= (const InputManagerSDL&);
};

#endif

/* EOF */
