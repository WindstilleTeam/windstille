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

#include "input_bindings.hpp"

#include <algorithm>

#include <logmich/log.hpp>
#include <prio/reader.hpp>

#include "controller_description.hpp"
#include "input_manager.hpp"

using namespace prio;

namespace wstinput {

// FIXME: this should be configurable and per axis
constexpr int g_dead_zone = 0;

InputBindings::InputBindings(InputManagerSDL& manager) :
  m_manager(manager),
  m_joystick_button_bindings(),
  m_joystick_button_axis_bindings(),
  m_joystick_axis_bindings(),
  m_joystick_axis_button_bindings(),
  m_keyboard_button_bindings(),
  m_keyboard_axis_bindings(),
  m_mouse_button_bindings(),
  m_mouse_motion_bindings(),
  m_mouse_motion_ball_bindings(),
  m_wiimote_button_bindings(),
  m_wiimote_axis_bindings()
{
}

void
InputBindings::load(std::filesystem::path const& filename,
                    ControllerDescription const& controller_description)
{
  ReaderDocument doc = ReaderDocument::from_file(filename);

  log_info("InputManager: {}", filename.string());

  if (doc.get_name() != "windstille-controller") {
    std::ostringstream msg;
    msg << "'" << filename << "' is not a windstille-controller file";
    throw std::runtime_error(msg.str());
  }

  ReaderMapping const& reader = doc.get_mapping();
  for (auto const& key : reader.get_keys())
  {
    if (key.ends_with("-button"))
    {
      ReaderObject button_obj;
      reader.read(key, button_obj);
      ReaderMapping const& button_map = button_obj.get_mapping();

      if (button_obj.get_name() == "joystick-button") {
        int device = 0;
        int button = 0;

        button_map.read("device", device);
        button_map.read("button", button);

        bind_joystick_button(controller_description.get_definition(key).id,
                             device, button);
      } else if (button_obj.get_name() == "joystick-axis-button") {
        int  device;
        int  axis;
        bool up;

        button_map.read("device", device);
        button_map.read("axis", axis);
        button_map.read("up", up);

        bind_joystick_axis_button(controller_description.get_definition(key).id,
                                  device, axis, up);
      } else if (button_obj.get_name() == "wiimote-button") {
        int device = 0;
        int button = 0;

        button_map.read("device", device);
        button_map.read("button", button);

        bind_wiimote_button(controller_description.get_definition(key).id,
                            device, button);
      } else if (button_obj.get_name() == "keyboard-button") {
        std::string key_text;
        button_map.read("key", key_text);

        bind_keyboard_button(controller_description.get_definition(key).id,
                             m_manager.string_to_keyid(key_text));
      } else {
        log_error("InputManagerSDL: Unknown tag: {}", button_obj.get_name());
      }
    }
    else if (key.ends_with("-axis"))
    {
      ReaderObject axis_obj;
      reader.read(key, axis_obj);
      ReaderMapping const& axis_map = axis_obj.get_mapping();

      if (axis_obj.get_name() == "joystick-axis")
      {
        int  device = 0;
        int  axis   = 0;
        bool invert = false;

        axis_map.read("device", device);
        axis_map.read("axis",   axis);
        axis_map.read("invert", invert);

        bind_joystick_axis(controller_description.get_definition(key).id,
                           device, axis, invert);
      }
      else if (axis_obj.get_name() == "keyboard-axis")
      {
        std::string minus;
        std::string plus;

        axis_map.read("minus", minus);
        axis_map.read("plus",  plus);

        bind_keyboard_axis(controller_description.get_definition(key).id,
                           m_manager.string_to_keyid(minus), m_manager.string_to_keyid(plus));
      }
      else if (axis_obj.get_name() == "wiimote-axis")
      {
        int  device = 0;
        int  axis   = 0;

        axis_map.read("device", device);
        axis_map.read("axis",   axis);

        bind_wiimote_axis(controller_description.get_definition(key).id,
                          device, axis);
      }
      else
      {
        log_error("InputManagerSDL: Unknown tag: {}", axis_obj.get_name());
      }
    }
  }
}

void
InputBindings::bind_mouse_button(int event, int device, int button)
{
  MouseButtonBinding binding;

  binding.event  = event;
  binding.device = device;
  binding.button = button;

  m_mouse_button_bindings.push_back(binding);
}

void
InputBindings::bind_mouse_motion(int event, int device, int axis)
{
  MouseMotionBinding binding;

  binding.event  = event;
  binding.device = device;
  binding.axis = axis;

  m_mouse_motion_bindings.push_back(binding);
}

void
InputBindings::bind_mouse_motion_ball(int event, int device, int axis)
{
  MouseMotionBallBinding binding;

  binding.event  = event;
  binding.device = device;
  binding.axis = axis;

  m_mouse_motion_ball_bindings.push_back(binding);
}

void
InputBindings::bind_joystick_hat_axis(int /*event*/, int /*device*/, int /*axis*/)
{
  log_not_implemented();
}

void
InputBindings::bind_joystick_button_axis(int event, int device, int minus, int plus)
{
  m_manager.ensure_open_joystick(device);

  JoystickButtonAxisBinding binding;

  binding.event  = event;
  binding.device = device;
  binding.minus  = minus;
  binding.plus   = plus;

  m_joystick_button_axis_bindings.push_back(binding);
}

void
InputBindings::bind_joystick_axis(int event, int device, int axis, bool invert)
{
  m_manager.ensure_open_joystick(device);

  JoystickAxisBinding binding;

  binding.event  = event;
  binding.device = device;
  binding.axis   = axis;
  binding.invert = invert;

  m_joystick_axis_bindings.push_back(binding);
}

void
InputBindings::bind_joystick_button(int event, int device, int button)
{
  m_manager.ensure_open_joystick(device);

  JoystickButtonBinding binding;

  binding.event  = event;
  binding.device = device;
  binding.button = button;

  m_joystick_button_bindings.push_back(binding);
}

void
InputBindings::bind_joystick_axis_button(int event, int device, int axis, bool up)
{
  m_manager.ensure_open_joystick(device);

  JoystickAxisButtonBinding binding;

  binding.event  = event;
  binding.device = device;
  binding.axis   = axis;
  binding.up   = up;

  m_joystick_axis_button_bindings.push_back(binding);
}

void
InputBindings::bind_keyboard_button(int event, SDL_Scancode key)
{
  KeyboardButtonBinding binding;

  binding.event = event;
  binding.key   = key;

  m_keyboard_button_bindings.push_back(binding);
}

void
InputBindings::bind_keyboard_axis(int event, SDL_Scancode minus, SDL_Scancode plus)
{
  KeyboardAxisBinding binding;

  binding.event = event;
  binding.minus = minus;
  binding.plus  = plus;

  m_keyboard_axis_bindings.push_back(binding);
}

void
InputBindings::bind_wiimote_button(int event, int device, int button)
{
  WiimoteButtonBinding binding;

  binding.event  = event;
  binding.device = device;
  binding.button = button;

  m_wiimote_button_bindings.push_back(binding);
}

void
InputBindings::bind_wiimote_axis(int event, int device, int axis)
{
  WiimoteAxisBinding binding;

  binding.event  = event;
  binding.device = device;
  binding.axis   = axis;

  m_wiimote_axis_bindings.push_back(binding);
}

void
InputBindings::clear()
{
  m_joystick_button_bindings.clear();
  m_joystick_axis_bindings.clear();
  m_joystick_button_axis_bindings.clear();
  m_joystick_axis_button_bindings.clear();

  m_keyboard_button_bindings.clear();
  m_keyboard_axis_bindings.clear();

  m_mouse_button_bindings.clear();

  m_wiimote_button_bindings.clear();
  m_wiimote_axis_bindings.clear();
}

void
InputBindings::dispatch_event(SDL_Event const& event, Controller& controller) const
{
  switch(event.type)
  {
    case SDL_TEXTINPUT: {
      std::array<char, 32> text;
      std::copy_n(event.text.text, text.size(), text.begin());
      controller.add_text_event(0, text);
      break;
    }

    case SDL_TEXTEDITING: {
      std::array<char, 32> text;
      std::copy_n(event.text.text, text.size(), text.begin());
      controller.add_text_edit_event(0, text, event.edit.start, event.edit.length);
      break;
    }

    case SDL_KEYUP:
    case SDL_KEYDOWN:
      if (m_manager.is_text_input_active()) {
        controller.add_keyboard_event(event.key);
      } else {
        dispatch_key_event(event.key, controller);
      }
      break;

    case SDL_MOUSEMOTION:
      dispatch_mouse_motion_event(event.motion, controller);
      break;

    case SDL_MOUSEBUTTONDOWN:
    case SDL_MOUSEBUTTONUP:
      dispatch_mouse_button_event(event.button, controller);
      break;

    case SDL_MOUSEWHEEL:
      dispatch_mouse_wheel_event(event.wheel, controller);
      break;

    case SDL_JOYAXISMOTION:
      dispatch_joy_axis_event(event.jaxis, controller);
      break;

    case SDL_JOYBALLMOTION:
      // event.jball
      break;

    case SDL_JOYHATMOTION:
      // event.jhat
      break;

    case SDL_JOYBUTTONUP:
    case SDL_JOYBUTTONDOWN:
      dispatch_joy_button_event(event.jbutton, controller);
      break;

    case SDL_QUIT:
    case SDL_WINDOWEVENT:
    case SDL_SYSWMEVENT:
    case SDL_KEYMAPCHANGED:
    case SDL_JOYDEVICEADDED:
    case SDL_JOYDEVICEREMOVED:
    case SDL_CONTROLLERDEVICEREMOVED:
    case SDL_CONTROLLERDEVICEREMAPPED:
    case SDL_CONTROLLERAXISMOTION:
    case SDL_CONTROLLERBUTTONUP:
    case SDL_CONTROLLERBUTTONDOWN:
    case SDL_CONTROLLERDEVICEADDED:
    case SDL_CLIPBOARDUPDATE:
    case SDL_DROPFILE:
    case SDL_DROPTEXT:
    case SDL_DROPBEGIN:
    case SDL_DROPCOMPLETE:
    case SDL_AUDIODEVICEADDED:
    case SDL_AUDIODEVICEREMOVED:
    case SDL_RENDER_TARGETS_RESET:
    case SDL_RENDER_DEVICE_RESET:
    case SDL_USEREVENT:
      break;

    default:
      log_debug("InputManagerSDL: unknown event: ", event.type);
      break;
  }
}

void
InputBindings::dispatch_key_event(const SDL_KeyboardEvent& event, Controller& controller) const
{
  // Dynamic bindings
  for (std::vector<KeyboardButtonBinding>::const_iterator i = m_keyboard_button_bindings.begin();
       i != m_keyboard_button_bindings.end();
       ++i)
  {
    if (event.keysym.scancode == i->key)
    {
      controller.add_button_event(i->event, event.state);
    }
  }

  const Uint8* keystate = SDL_GetKeyboardState(nullptr);

  for (std::vector<KeyboardAxisBinding>::const_iterator i = m_keyboard_axis_bindings.begin();
       i != m_keyboard_axis_bindings.end();
       ++i)
  {
    if (event.keysym.scancode == i->minus)
    {
      if (event.state)
        controller.add_axis_event(i->event, -1.0f);
      else if (!keystate[i->plus])
        controller.add_axis_event(i->event, 0.0f);
    }
    else if (event.keysym.scancode == i->plus)
    {
      if (event.state)
      {
        controller.add_axis_event(i->event, 1.0f);
      }
      else if (!keystate[i->minus])
      {
        controller.add_axis_event(i->event, 0.0f);
      }
    }
  }
}

void
InputBindings::dispatch_mouse_button_event(const SDL_MouseButtonEvent& button, Controller& controller) const
{
  for (std::vector<MouseButtonBinding>::const_iterator i = m_mouse_button_bindings.begin();
       i != m_mouse_button_bindings.end();
       ++i)
  {
    if (button.button == i->button)
    {
      controller.add_button_event(i->event, button.state);
    }
  }
}

void
InputBindings::dispatch_mouse_motion_event(SDL_MouseMotionEvent const& motion, Controller& controller) const
{
  for (MouseMotionBinding const& binding : m_mouse_motion_bindings)
  {
    if (static_cast<int>(motion.which) == binding.device)
    {
      if (binding.axis == 0) {
        controller.add_pointer_event(binding.event, static_cast<float>(motion.x));
      } else if (binding.axis == 1) {
        controller.add_pointer_event(binding.event, static_cast<float>(motion.y));
      } else {
        log_error("unknown axis in binding: {}", binding.axis);
      }
    }
  }

  for (MouseMotionBallBinding const& binding : m_mouse_motion_ball_bindings)
  {
    if (binding.axis == 0) {
      controller.add_ball_event(binding.event, static_cast<float>(motion.xrel));
    } else if (binding.axis == 1) {
      controller.add_ball_event(binding.event, static_cast<float>(motion.yrel));
    } else {
      log_error("unknown axis in binding: {}", binding.axis);
    }
  }
}

void
InputBindings::dispatch_mouse_wheel_event(SDL_MouseWheelEvent const& wheel, Controller& controller) const
{

}

void
InputBindings::dispatch_joy_button_event(const SDL_JoyButtonEvent& button, Controller& controller) const
{
  for (std::vector<JoystickButtonBinding>::const_iterator i = m_joystick_button_bindings.begin();
       i != m_joystick_button_bindings.end();
       ++i)
  {
    if (button.which  == i->device &&
        button.button == i->button)
    {
      controller.add_button_event(i->event, button.state);
    }
  }

  for (std::vector<JoystickButtonAxisBinding>::const_iterator i = m_joystick_button_axis_bindings.begin();
       i != m_joystick_button_axis_bindings.end();
       ++i)
  {
    if (button.which  == i->device)
    {
      if (button.button == i->minus)
      {
        controller.add_axis_event(i->event, button.state ? -1.0f : 0.0f);
      }
      else if (button.button == i->plus)
      {
        controller.add_axis_event(i->event, button.state ?  1.0f : 0.0f);
      }
    }
  }
}

void
InputBindings::dispatch_joy_axis_event(const SDL_JoyAxisEvent& event, Controller& controller) const
{
  for (std::vector<JoystickAxisBinding>::const_iterator i = m_joystick_axis_bindings.begin();
       i != m_joystick_axis_bindings.end();
       ++i)
  {
    if (event.which  == i->device &&
        event.axis   == i->axis)
    {
      if (abs(event.value) > g_dead_zone)
      {
        controller.add_axis_event(i->event, static_cast<float>(event.value) / (i->invert ? -32768.0f : 32768.0f));
      }
      else
      {
        controller.add_axis_event(i->event, 0);
      }
    }
  }

  for(std::vector<JoystickAxisButtonBinding>::const_iterator i = m_joystick_axis_button_bindings.begin();
      i != m_joystick_axis_button_bindings.end();
      ++i)
  {
    if (event.which == i->device &&
        event.axis  == i->axis)
    {
      if (i->up)
      { // signal button press when axis is up
        if (event.value < -g_dead_zone)
          controller.add_button_event(i->event, true);
        else
          controller.add_button_event(i->event, false);
      }
      else
      { // signal button press when axis is down
        if (event.value > g_dead_zone)
          controller.add_button_event(i->event, true);
        else
          controller.add_button_event(i->event, false);
      }
    }
  }
}

} // namespace wstinput

/* EOF */
