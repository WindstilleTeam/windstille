/*  $Id$
**   __      __ __             ___        __   __ __   __
**  /  \    /  \__| ____    __| _/_______/  |_|__|  | |  |   ____
**  \   \/\/   /  |/    \  / __ |/  ___/\   __\  |  | |  | _/ __ \
**   \        /|  |   |  \/ /_/ |\___ \  |  | |  |  |_|  |_\  ___/
**    \__/\  / |__|___|  /\____ /____  > |__| |__|____/____/\___  >
**         \/          \/      \/    \/                         \/
**  Copyright (C) 2005,2007 Ingo Ruhnke <grumbel@gmx.de>
**
**  This program is free software; you can redistribute it and/or
**  modify it under the terms of the GNU General Public License
**  as published by the Free Software Foundation; either version 2
**  of the License, or (at your option) any later version.
**
**  This program is distributed in the hope that it will be useful,
**  but WITHOUT ANY WARRANTY; without even the implied warranty of
**  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
**  GNU General Public License for more details.
** 
**  You should have received a copy of the GNU General Public License
**  along with this program; if not, write to the Free Software
**  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA
**  02111-1307, USA.
*/

#ifndef HEADER_INPUT_MANAGER_SDL_HPP
#define HEADER_INPUT_MANAGER_SDL_HPP

#include <SDL.h>
#include <memory>
#include "input_manager_impl.hpp"

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
class InputManagerSDL : public InputManagerImpl
{
private:
  static InputManagerSDL* current_;
public:
  static InputManagerSDL* current() { return current_; }

  InputManagerSDL();
  virtual ~InputManagerSDL();

  void load(const std::string& filename);

  void update(float delta);

  void bind_joystick_hat_axis(int event, int device, int axis);

  void bind_joystick_axis(int event, int device, int axis, bool invert);
  void bind_joystick_button_axis(int event, int device, int minus, int plus);
  void bind_joystick_button(int event, int device, int button);

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

  std::auto_ptr<InputManagerSDLImpl> impl;

  InputManagerSDL (const InputManagerSDL&);
  InputManagerSDL& operator= (const InputManagerSDL&);
};

#endif

/* EOF */
