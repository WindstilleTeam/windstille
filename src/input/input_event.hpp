/*
**  Windstille - A Sci-Fi Action-Adventure Game
**  Copyright (C) 2000,2005 Ingo Ruhnke <grumbel@gmail.com>
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

#ifndef HEADER_WINDSTILLE_INPUT_INPUT_EVENT_HPP
#define HEADER_WINDSTILLE_INPUT_INPUT_EVENT_HPP

#include <vector>

enum InputEventType { BUTTON_EVENT, AXIS_EVENT, BALL_EVENT, KEYBOARD_EVENT };

/** Used for textual input */
struct KeyboardEvent
{
  enum KeyType { LETTER, SPECIAL } key_type;
  int code;
};

struct ButtonEvent
{
  int name;

  /** true if down, false if up */
  bool down;

  bool is_down() const { return down; }
  bool is_up()   const { return !down; }
};

struct BallEvent
{
  int   name;
  float pos;
  float get_pos() const { return pos; }
};

struct AxisEvent
{
  int name;

  /** Pos can be in range from [-1.0, 1.0], some axis will only use [0,1.0] */
  float pos;

  float get_pos() const { return pos; }
};

struct InputEvent
{
  InputEventType type;

  union
  {
    struct ButtonEvent   button;
    struct AxisEvent     axis;
    struct KeyboardEvent keyboard;
    struct BallEvent     ball;
  };
};

typedef std::vector<InputEvent> InputEventLst;

#endif

/* EOF */
