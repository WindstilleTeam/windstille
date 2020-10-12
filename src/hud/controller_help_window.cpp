/*
**  Windstille - A Sci-Fi Action-Adventure Game
**  Copyright (C) 2005 Ingo Ruhnke <grumbel@gmail.com>
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

#include "hud/controller_help_window.hpp"

#include <wstinput/input_manager.hpp>

#include "app/app.hpp"
#include "app/controller_def.hpp"
#include "display/color.hpp"
#include "display/display.hpp"
#include "math/rect.hpp"

ControllerHelpWindow::ControllerHelpWindow()
{
}

void
ControllerHelpWindow::draw_button(const Vector2f& pos, bool pressed)
{
  if (pressed)
  {
    Display::fill_circle(pos, 12.0f, Color(0.8f, 0.0f, 0.0f));
    //Display::draw_circle(pos, 12.0f, Color(1.0f, 0.0f, 0.0f));
  }
  else
  {
    Display::fill_circle(pos, 12.0f, Color(1.0f, 1.0f, 1.0f, 0.2f));
    //Display::draw_circle(pos, 12.0f, Color(1.0f, 1.0f, 1.0f, 0.5f));
  }
}

void
ControllerHelpWindow::draw_stick(const Vector2f& pos, bool pressed, float x, float y)
{
  Size size(75, 75);
  Rectf rect(pos - glm::vec2(static_cast<float>(size.width()), static_cast<float>(size.height())) / 2.0f, geom::fsize(size));

  Display::fill_rounded_rect(rect, 10.0f, Color(1.0f, 1.0f, 1.0f, 0.2f));
  //Display::draw_rounded_rect(rect, 10.0f, Color(1.0f, 1.0f, 1.0f, 0.5f));

  Vector2f stick_pos(rect.left() + rect.width()/2  + x * (rect.width()-16.0f)/2,
                     rect.top()  + rect.height()/2 + y * (rect.width()-16.0f)/2);

  if (pressed)
    Display::fill_circle(stick_pos, 10.0f, Color(0.8f, 0, 0));
  else
    Display::fill_circle(stick_pos, 10.0f, Color(1.0f, 0, 0, 0.25f));

  Display::draw_circle(stick_pos, 10.0f, Color(1.0f, 0, 0));
}

void
ControllerHelpWindow::draw_trigger(const Vector2f& pos, float value)
{
  Sizef size(60, 20);
  Rectf rect(pos - Vector2f(size.width() / 2, size.height() / 2), size);

  Display::fill_rect(rect,  Color(1.0f, 1.0f, 1.0f, 0.2f));

  if (value > 0.0f)
    Display::fill_rect(Rectf(Vector2f(rect.left(), rect.top()),
                             Sizef(rect.width() * value,
                                   rect.height())),
                       Color(1.0f, 0.0f, 0.0f));

  //Display::draw_rect(rect, Color(1.0f, 1.0f, 1.0f, 0.5f));
}

void
ControllerHelpWindow::draw()
{
  const Controller& controller = g_app.input().get_controller();

  Vector2f pos(static_cast<float>(Display::get_width())  - 350.0f - 16.0f,
               static_cast<float>(Display::get_height()) - 200.0f - 16.0f);

  Display::fill_rounded_rect(Rectf(pos, Sizef(350, 200)), 10.0f, Color(0.0f, 0.0f, 0.25f, 0.9f));
  Display::draw_rounded_rect(Rectf(pos, Sizef(350, 200)), 10.0f, Color(1.0f, 1.0f, 1.0f,  0.5f));

  draw_stick(pos + Vector2f(50, 75),
             controller.get_button_state(LEFT_STICK_BUTTON),
             controller.get_axis_state(X_AXIS),
             controller.get_axis_state(Y_AXIS));

  draw_stick(pos + Vector2f(250, 150),
             controller.get_button_state(RIGHT_STICK_BUTTON),
             controller.get_axis_state(X2_AXIS),
             controller.get_axis_state(Y2_AXIS));

  draw_trigger(pos + Vector2f(45,  20), controller.get_trigger_state(LEFT_TRIGGER_AXIS));
  draw_trigger(pos + Vector2f(305, 20), controller.get_trigger_state(RIGHT_TRIGGER_AXIS));

  draw_button(pos + Vector2f(125, 75), controller.get_button_state(SELECT_BUTTON));
  draw_button(pos + Vector2f(225, 75), controller.get_button_state(START_BUTTON));

  draw_button(pos + Vector2f(100, 22), controller.get_button_state(LEFT_SHOULDER_BUTTON));
  draw_button(pos + Vector2f(250, 22), controller.get_button_state(RIGHT_SHOULDER_BUTTON));

  Vector2f face_pos(300, 75);
  draw_button(pos + face_pos + Vector2f(  0,  25), controller.get_button_state(PRIMARY_BUTTON));
  draw_button(pos + face_pos + Vector2f (25,   0), controller.get_button_state(SECONDARY_BUTTON));
  draw_button(pos + face_pos + Vector2f(-25,   0), controller.get_button_state(TERTIARY_BUTTON));
  draw_button(pos + face_pos + Vector2f(  0, -25), controller.get_button_state(QUATERNARY_BUTTON));

  Vector2f dpad_pos(100, 150);
  draw_button(pos + dpad_pos + Vector2f(  0,  25), controller.get_button_state(MENU_DOWN_BUTTON));
  draw_button(pos + dpad_pos + Vector2f (25,   0), controller.get_button_state(MENU_RIGHT_BUTTON));
  draw_button(pos + dpad_pos + Vector2f(-25,   0), controller.get_button_state(MENU_LEFT_BUTTON));
  draw_button(pos + dpad_pos + Vector2f(  0, -25), controller.get_button_state(MENU_UP_BUTTON));
}

void
ControllerHelpWindow::update(float /*delta*/, const Controller& )
{
}

/* EOF */
