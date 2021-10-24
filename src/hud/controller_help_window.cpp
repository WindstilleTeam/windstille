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

#include <geom/geom.hpp>
#include <wstinput/input_manager.hpp>

#include "app/app.hpp"
#include "app/controller_def.hpp"
#include <surf/color.hpp>
#include <wstdisplay/graphics_context.hpp>

ControllerHelpWindow::ControllerHelpWindow()
{
}

void
ControllerHelpWindow::draw_button(wstdisplay::GraphicsContext& gc, const glm::vec2& pos, bool pressed)
{
  if (pressed)
  {
    gc.fill_circle(pos, 12.0f, surf::Color(0.8f, 0.0f, 0.0f));
    //gc.draw_circle(pos, 12.0f, surf::Color(1.0f, 0.0f, 0.0f));
  }
  else
  {
    gc.fill_circle(pos, 12.0f, surf::Color(1.0f, 1.0f, 1.0f, 0.2f));
    //gc.draw_circle(pos, 12.0f, surf::Color(1.0f, 1.0f, 1.0f, 0.5f));
  }
}

void
ControllerHelpWindow::draw_stick(wstdisplay::GraphicsContext& gc, const glm::vec2& pos, bool pressed, float x, float y)
{
  geom::isize size(75, 75);
  geom::frect rect(pos - glm::vec2(static_cast<float>(size.width()), static_cast<float>(size.height())) / 2.0f, geom::fsize(size));

  gc.fill_rounded_rect(rect, 10.0f, surf::Color(1.0f, 1.0f, 1.0f, 0.2f));
  //gc.draw_rounded_rect(rect, 10.0f, surf::Color(1.0f, 1.0f, 1.0f, 0.5f));

  glm::vec2 stick_pos(rect.left() + rect.width()/2  + x * (rect.width()-16.0f)/2,
                     rect.top()  + rect.height()/2 + y * (rect.width()-16.0f)/2);

  if (pressed)
    gc.fill_circle(stick_pos, 10.0f, surf::Color(0.8f, 0, 0));
  else
    gc.fill_circle(stick_pos, 10.0f, surf::Color(1.0f, 0, 0, 0.25f));

  gc.draw_circle(stick_pos, 10.0f, surf::Color(1.0f, 0, 0));
}

void
ControllerHelpWindow::draw_trigger(wstdisplay::GraphicsContext& gc, const glm::vec2& pos, float value)
{
  geom::fsize size(60, 20);
  geom::frect rect(pos - glm::vec2(size.width() / 2, size.height() / 2), size);

  gc.fill_rect(rect,  surf::Color(1.0f, 1.0f, 1.0f, 0.2f));

  if (value > 0.0f)
    gc.fill_rect(geom::frect(glm::vec2(rect.left(), rect.top()),
                             geom::fsize(rect.width() * value,
                                   rect.height())),
                       surf::Color(1.0f, 0.0f, 0.0f));

  //gc.draw_rect(rect, surf::Color(1.0f, 1.0f, 1.0f, 0.5f));
}

void
ControllerHelpWindow::draw(wstdisplay::GraphicsContext& gc)
{
  wstinput::Controller const& controller = g_app.input().get_controller();

  glm::vec2 pos(static_cast<float>(gc.size().width())  - 350.0f - 16.0f,
               static_cast<float>(gc.size().height()) - 200.0f - 16.0f);

  gc.fill_rounded_rect(geom::frect(pos, geom::fsize(350, 200)), 10.0f, surf::Color(0.0f, 0.0f, 0.25f, 0.9f));
  gc.draw_rounded_rect(geom::frect(pos, geom::fsize(350, 200)), 10.0f, surf::Color(1.0f, 1.0f, 1.0f,  0.5f));

  draw_stick(gc,
             pos + glm::vec2(50, 75),
             controller.get_button_state(LEFT_STICK_BUTTON),
             controller.get_axis_state(X_AXIS),
             controller.get_axis_state(Y_AXIS));

  draw_stick(gc,
             pos + glm::vec2(250, 150),
             controller.get_button_state(RIGHT_STICK_BUTTON),
             controller.get_axis_state(X2_AXIS),
             controller.get_axis_state(Y2_AXIS));

  draw_trigger(gc, pos + glm::vec2(45,  20), controller.get_trigger_state(LEFT_TRIGGER_AXIS));
  draw_trigger(gc, pos + glm::vec2(305, 20), controller.get_trigger_state(RIGHT_TRIGGER_AXIS));

  draw_button(gc, pos + glm::vec2(125, 75), controller.get_button_state(SELECT_BUTTON));
  draw_button(gc, pos + glm::vec2(225, 75), controller.get_button_state(START_BUTTON));

  draw_button(gc, pos + glm::vec2(100, 22), controller.get_button_state(LEFT_SHOULDER_BUTTON));
  draw_button(gc, pos + glm::vec2(250, 22), controller.get_button_state(RIGHT_SHOULDER_BUTTON));

  glm::vec2 face_pos(300, 75);
  draw_button(gc, pos + face_pos + glm::vec2(  0,  25), controller.get_button_state(PRIMARY_BUTTON));
  draw_button(gc, pos + face_pos + glm::vec2 (25,   0), controller.get_button_state(SECONDARY_BUTTON));
  draw_button(gc, pos + face_pos + glm::vec2(-25,   0), controller.get_button_state(TERTIARY_BUTTON));
  draw_button(gc, pos + face_pos + glm::vec2(  0, -25), controller.get_button_state(QUATERNARY_BUTTON));

  glm::vec2 dpad_pos(100, 150);
  draw_button(gc, pos + dpad_pos + glm::vec2(  0,  25), controller.get_button_state(MENU_DOWN_BUTTON));
  draw_button(gc, pos + dpad_pos + glm::vec2 (25,   0), controller.get_button_state(MENU_RIGHT_BUTTON));
  draw_button(gc, pos + dpad_pos + glm::vec2(-25,   0), controller.get_button_state(MENU_LEFT_BUTTON));
  draw_button(gc, pos + dpad_pos + glm::vec2(  0, -25), controller.get_button_state(MENU_UP_BUTTON));
}

void
ControllerHelpWindow::update(float /*delta*/, wstinput::Controller const& controller )
{
}

/* EOF */
