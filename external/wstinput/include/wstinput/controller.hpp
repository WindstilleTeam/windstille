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

#ifndef HEADER_WINDSTILLE_INPUT_CONTROLLER_HPP
#define HEADER_WINDSTILLE_INPUT_CONTROLLER_HPP

#include <vector>

#include "input_event.hpp"

namespace wstinput {

/** The Controller class presents the current state of the controller
    and the input events that occurred on the controller since the
    last update */
class Controller final
{
private:
  union State {
    enum { BUTTON_STATE, BALL_STATE, AXIS_STATE, POINTER_STATE } type;
    bool  button;
    float axis;
    float ball;
    float pointer;
  };

public:
  Controller(size_t size = 0);

  float get_trigger_state(int name) const;
  float get_axis_state(int name, bool use_deadzone = true) const;
  bool get_button_state(int name) const;
  float get_ball_state(int name) const;
  float get_pointer_state(int name) const;

  InputEventLst const& get_events() const;

  /** Convenience function that searches for a button down event for
      the given button */
  bool button_was_pressed(int name) const;

  /** Convenience function that searches for a AxisMove event that
      pushed the axis up */
  bool axis_was_pressed_up(int name) const;

  /** Convenience function that searches for a AxisMove event that
      pushed the axis down */
  bool axis_was_pressed_down(int name) const;

  void set_axis_state(int name, float pos);
  void set_button_state(int name, bool down);
  void set_ball_state(int name, float delta);
  void set_pointer_state(int name, float pos);

  void add_axis_event(int name, float pos);
  void add_ball_event(int name, float pos);
  void add_pointer_event(int name, float pos);
  void add_button_event(int name, bool down);
  void add_text_event(int name, std::array<char, 32> const& text);
  void add_text_edit_event(int , std::array<char, 32> const& text, int start, int length);
  void add_keyboard_event(SDL_KeyboardEvent const& key);

  void clear();

private:
  void add_event(const InputEvent& event);

private:
  std::vector<State> m_states;
  InputEventLst m_events;

public:
  Controller(const Controller&) = delete;
  Controller& operator=(const Controller&) = delete;
};

} // namespace wstinput

#endif

/* EOF */
