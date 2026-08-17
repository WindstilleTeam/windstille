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

#include <sstream>

#include <logmich/log.hpp>
#include <prio/reader.hpp>

#include "input_manager.hpp"
#ifdef HAVE_CWIID
#  include "wiimote.hpp"
#endif

using namespace prio;

namespace wstinput {

InputManagerSDL::InputManagerSDL(ControllerDescription const& controller_description) :
  m_controller_description(controller_description),
  m_controller(controller_description.get_max_id() + 1),
  m_bindings(*this),
  m_joysticks(),
  m_keyidmapping()
{
  log_debug("Keyboard keys:");
  for (int i = 0; i < SDL_NUM_SCANCODES; ++i) {
    const char* key_name = SDL_GetScancodeName(static_cast<SDL_Scancode>(i));
    m_keyidmapping[key_name] = static_cast<SDL_Scancode>(i);
    // FIXME: Make the keynames somewhere user visible so that users can use them
    log_debug("  {}", key_name);
  }

  stop_text_input();

#ifdef HAVE_CWIID
  // FIXME: doesn't really belong here
  Wiimote::init();

  if (wiimote && config.get<bool>("wiimote").get())
    wiimote->connect();

#endif // HAVE_CWIID
}

InputManagerSDL::~InputManagerSDL()
{
#ifdef HAVE_CWIID
  Wiimote::deinit();
#endif
}

void
InputManagerSDL::load(std::filesystem::path const& filename)
{
  m_bindings.load(filename, m_controller_description);
}

std::string
InputManagerSDL::keyid_to_string(SDL_Scancode id) const
{
  return SDL_GetKeyName(id);
}

SDL_Scancode
InputManagerSDL::string_to_keyid(const std::string& str) const
{
  auto const it = m_keyidmapping.find(str);
  if (it == m_keyidmapping.end())
  {
    std::ostringstream msg;
    msg << "key lookup failure for '" << str << "'";
    throw std::runtime_error(msg.str());
  }
  else
  {
    return it->second;
  }
}

void
InputManagerSDL::ensure_open_joystick(int device)
{
  if (device >= int(m_joysticks.size()))
    m_joysticks.resize(device + 1, nullptr);

  if (!m_joysticks[device])
  {
    if (SDL_Joystick* joystick = SDL_JoystickOpen(device))
    {
      m_joysticks[device] = joystick;
    }
    else
    {
      log_error("InputManagerSDL: Couldn't open joystick device: {}", device);
    }
  }
}

void
InputManagerSDL::on_event(const SDL_Event& event)
{
  m_bindings.dispatch_event(event, m_controller);
}

void
InputManagerSDL::update(float /*delta*/)
{
#ifdef HAVE_CWIID
  if (wiimote && wiimote->is_connected())
  {
    // Check for new events from the Wiimote
    std::vector<WiimoteEvent> events = wiimote->pop_events();
    for(std::vector<WiimoteEvent>::iterator i = events.begin(); i != events.end(); ++i)
    {
      WiimoteEvent& event = *i;
      if (event.type == WiimoteEvent::WIIMOTE_BUTTON_EVENT)
      {
        for (std::vector<WiimoteButtonBinding>::const_iterator j = m_wiimote_button_bindings.begin();
             j != m_wiimote_button_bindings.end();
             ++j)
        {
          if (event.button.device == j->device &&
              event.button.button == j->button)
          {
            add_button_event(j->event, event.button.down);
          }
        }
      }
      else if (event.type == WiimoteEvent::WIIMOTE_AXIS_EVENT)
      {
        for (std::vector<WiimoteAxisBinding>::const_iterator j = m_wiimote_axis_bindings.begin();
             j != m_wiimote_axis_bindings.end();
             ++j)
        {
          if (event.axis.device == j->device &&
              event.axis.axis == j->axis)
          {
            add_axis_event(j->event, event.axis.pos);
          }
        }
      }
      else if (event.type == WiimoteEvent::WIIMOTE_ACC_EVENT)
      {
        if (event.acc.accelerometer == 0)
        {
          if (0)
            printf("%d - %6.3f %6.3f %6.3f\n",
                   event.acc.accelerometer,
                   event.acc.x,
                   event.acc.y,
                   event.acc.z);

          float roll = atanf(static_cast<float>(event.acc.x / event.acc.z));
          if (event.acc.z <= 0.0) {
            roll += math::pi * ((event.acc.x > 0.0f) ? 1.0f : -1.0f);
          }
          roll *= -1;

          float pitch = atanf(event.acc.y / event.acc.z * cosf(roll));

          add_axis_event(X2_AXIS, math::mid(-1.0f, -float(pitch / M_PI), 1.0f));
          add_axis_event(Y2_AXIS, math::mid(-1.0f, -float(roll  / M_PI), 1.0f));

          log_debug("{:6.3f} {:6.3f}", pitch, roll);
        }
      }
      else
      {
        assert(!"Never reached");
      }
    }
  }
#endif
}

void
InputManagerSDL::clear()
{
  m_controller.clear();
}

void
InputManagerSDL::start_text_input()
{
  SDL_StartTextInput();
}

void
InputManagerSDL::stop_text_input()
{
  SDL_StopTextInput();
}

bool
InputManagerSDL::is_text_input_active() const
{
  return SDL_IsTextInputActive();
}

} // namespace wstinput

/* EOF */
