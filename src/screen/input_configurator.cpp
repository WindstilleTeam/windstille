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

#include "screen/input_configurator.hpp"

#include <iostream>

#include <wstinput/controller.hpp>
#include <wstinput/input_manager.hpp>
#include <wstdisplay/graphics_context.hpp>
#include <wstdisplay/opengl_window.hpp>
#include <wstgui/screen_manager.hpp>
#include <wstgui/style.hpp>

#include "app/app.hpp"
#include "app/controller_def.hpp"
#include "font/fonts.hpp"

namespace windstille {

InputConfigurator::InputConfigurator()
  : items(),
    wait_for_plus(false),
    minus(),
    out(),
    area(g_app.style().get_font(),
         geom::frect(120.0f,
                     100.0f,
                     static_cast<float>(g_app.window().get_gc().size().width())  - 120.0f,
                     static_cast<float>(g_app.window().get_gc().size().height()) - 100.0f),
         false)
{
  out << "Input Configurator\n"
      << "------------------\n"
      << "Press Escape to ignore a setting\n"
      << std::endl;

  area.set_text(out.str());

  area.set_font(g_app.fonts().ttffont.get());

  add_configure_item(ConfigureItem::CONFIGURE_BUTTON, INVENTORY_BUTTON);
  add_configure_item(ConfigureItem::CONFIGURE_BUTTON, AIM_BUTTON);
  add_configure_item(ConfigureItem::CONFIGURE_BUTTON, PDA_BUTTON);
  add_configure_item(ConfigureItem::CONFIGURE_BUTTON, TERTIARY_BUTTON);
  add_configure_item(ConfigureItem::CONFIGURE_BUTTON, SECONDARY_BUTTON);
  add_configure_item(ConfigureItem::CONFIGURE_BUTTON, PRIMARY_BUTTON);

  add_configure_item(ConfigureItem::CONFIGURE_AXIS, Y_AXIS);
  add_configure_item(ConfigureItem::CONFIGURE_AXIS, X_AXIS);

  print_item();

  g_app.input().bindings().clear();
}

InputConfigurator::~InputConfigurator()
{
}

void
InputConfigurator::add_configure_item(ConfigureItem::Mode mode, int event_id)
{
  ConfigureItem item;

  item.mode     = mode;
  item.event_id = event_id;

  items.push_back(item);

}

void
InputConfigurator::draw(wstdisplay::GraphicsContext& gc)
{
  geom::frect rect(100.0f, 75.0f,
             static_cast<float>(gc.size().width())  - 100.0f,
             static_cast<float>(gc.size().height()) - 75.0f);

  gc.fill_rounded_rect(rect, 16.0f, surf::Color(0.3f, 0.3f, 0.5f, 0.5f));
  gc.draw_rounded_rect(rect, 16.0f, surf::Color(1.0f, 1.0f, 1.0f, 0.5f));
  area.draw(gc);
}

void
InputConfigurator::update(float , wstinput::Controller const& controller )
{

}

void
InputConfigurator::print_item()
{
  if (!items.empty())
  {
    ConfigureItem const& item = items.back();

    if (item.mode == ConfigureItem::CONFIGURE_AXIS)
    {
      out << "Configuring "
          << g_app.input().get_controller_description().get_definition(item.event_id).name
          << ": " << std::endl;
    }
    else if (item.mode == ConfigureItem::CONFIGURE_BUTTON)
    {
      out << "Configuring "
          << g_app.input().get_controller_description().get_definition(item.event_id).name
          << ": " << std::endl;
    }
  }

  area.set_text(out.str());
}

void
InputConfigurator::next_item()
{
  out << std::endl;
  items.pop_back();

  if (items.empty())
  {
    out << "Controller configuration is done, pressy any key to continue" << std::endl;
  }

  print_item();

  area.set_text(out.str());
}

void
InputConfigurator::handle_event(SDL_Event const& event)
{
  if (items.empty())
  {
    std::cout << "InputConfigurator: done" << std::endl;
    g_app.screen().pop_overlay();
    return;
  }


  switch(event.type)
  {
    case SDL_MOUSEMOTION:
      // event.motion:
      break;

    case SDL_MOUSEBUTTONDOWN:
      if (items.back().mode == ConfigureItem::CONFIGURE_BUTTON)
      {
        g_app.input().bindings().bind_mouse_button(items.back().event_id,
                                                      0, // SDL only supports one mouse
                                                      event.button.button);
        out << "(mouse-button (device " << 0 << ")\n"
            << "              (button " << int(event.button.button) << "))" << std::endl;
        next_item();
      }
      break;

    case SDL_MOUSEBUTTONUP:
      // event.button
      break;

    case SDL_JOYAXISMOTION:
      if (items.back().mode == ConfigureItem::CONFIGURE_AXIS && (event.jaxis.value > 16384 || event.jaxis.value < -16384))
      { // FIXME: This doesn't work well with analog Axis!
        g_app.input().bindings().bind_joystick_axis(items.back().event_id, event.jaxis.which, event.jaxis.axis, false);
        out << "(joystick-axis (device " << int(event.jaxis.which) << ")\n"
            << "               (axis   " << int(event.jaxis.axis) << "))" << std::endl;
        next_item();
      }
      else
      {
      }
      break;

    case SDL_JOYBALLMOTION:
      // event.jball
      break;

    case SDL_JOYHATMOTION:
      // event.jhat
      break;

    case SDL_JOYBUTTONUP:
      break;

    case SDL_JOYBUTTONDOWN:
      if (items.back().mode == ConfigureItem::CONFIGURE_BUTTON)
      {
        g_app.input().bindings().bind_joystick_button(items.back().event_id, event.jbutton.which, event.jbutton.button);
        out << "(joystick-button (device " << int(event.jbutton.which) << ")\n"
            << "                 (button " << int(event.jbutton.button) << "))" << std::endl;
        next_item();
      }
      else if (items.back().mode == ConfigureItem::CONFIGURE_AXIS)
      {
        if (wait_for_plus && minus.type == SDL_JOYBUTTONDOWN)
        {
          out << "(joystick-axis-button (minus " << g_app.input().keyid_to_string(minus.key.keysym.scancode) << ") "
              << "(plus  " << g_app.input().keyid_to_string(event.key.keysym.scancode) << "))" << std::endl;
          g_app.input().bindings().bind_joystick_button_axis(items.back().event_id, event.jbutton.which,
                                                                minus.jbutton.button, event.jbutton.button);
          next_item();
          wait_for_plus = false;
        }
        else if (!wait_for_plus)
        {
          out << "Press key for other direction" << std::endl;
          area.set_text(out.str());
          minus = event;
          wait_for_plus = true;
        }
      }
      break;

    case SDL_KEYUP:
      break;

    case SDL_KEYDOWN:
      if (event.key.keysym.sym == SDLK_ESCAPE)
      {
        std::cout << "InputConfigurator: abort" << std::endl;
        g_app.screen().pop_overlay();
        //next_item();
      }
      else if (event.key.keysym.sym == SDLK_RETURN)
      {
        std::cout << "Binding the enter key is not allowed" << std::endl;
      }
      else
      {
        if (items.back().mode == ConfigureItem::CONFIGURE_BUTTON)
        {
          g_app.input().bindings().bind_keyboard_button(items.back().event_id, event.key.keysym.scancode);
          out << "(keyboard-button (key " << g_app.input().keyid_to_string(event.key.keysym.scancode) << "))" << std::endl;
          next_item();
        }
        else if (items.back().mode == ConfigureItem::CONFIGURE_AXIS)
        {
          if (wait_for_plus && minus.type == SDL_KEYDOWN)
          {
            out << "(keyboard-axis (minus " << g_app.input().keyid_to_string(minus.key.keysym.scancode) << ") "
                << "(plus  " << g_app.input().keyid_to_string(event.key.keysym.scancode) << "))" << std::endl;
            g_app.input().bindings().bind_keyboard_axis(items.back().event_id, minus.key.keysym.scancode, event.key.keysym.scancode);
            next_item();
            wait_for_plus = false;
          }
          else if (!wait_for_plus)
          {
            out << "Print key for other direction" << std::endl;
            area.set_text(out.str());
            minus = event;
            wait_for_plus = true;
          }
        }
      }
      break;
  }
}

} // namespace windstille

/* EOF */
