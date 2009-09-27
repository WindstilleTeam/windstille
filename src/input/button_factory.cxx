//  $Id$
//
//  Pingus - A free Lemmings clone
//  Copyright (C) 2002 Ingo Ruhnke <grumbel@gmx.de>
//
//  This program is free software; you can redistribute it and/or
//  modify it under the terms of the GNU General Public License
//  as published by the Free Software Foundation; either version 2
//  of the License, or (at your option) any later version.
//
//  This program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
//
//  You should have received a copy of the GNU General Public License
//  along with this program; if not, write to the Free Software
//  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.

#include <ClanLib/Display/joystick.h>
#include <ClanLib/Display/keyboard.h>
#include <ClanLib/Core/System/clanstring.h>
#include "../guile.hxx"
#include "../feuerkraft_error.hxx"
#include "input_button.hxx"
#include "input_axis.hxx"
#include "input_button_input_device.hxx"
#include "axis_factory.hxx"
#include "axis_button.hxx"
#include "multi_button.hxx"
#include "button_factory.hxx"

InputButton* 
ButtonFactory::create(SCM lst)
{
  SCM sym = gh_car(lst);

  if (gh_equal_p(sym, gh_symbol2scm("joystick-button")))
    {
      return create_joystick_button(gh_cdr(lst));
    }
  else if (gh_equal_p(sym, gh_symbol2scm("keyboard-button")))
    {
      return create_keyboard_button(gh_cdr(lst));
    }
  else if (gh_equal_p(sym, gh_symbol2scm("axis-button")))
    {
      return create_axis_button(gh_cdr(lst));
    }
  else if (gh_equal_p(sym, gh_symbol2scm("multi-button")))
    {
      return create_multi_button(gh_cdr(lst));
    }
  else
    {
      throw FeuerkraftError("ButtonFactory::create: parse error: '"
                            + Guile::scm2string(lst) + "'");
    }
      
  return 0;
}

InputButton*
ButtonFactory::create_axis_button(SCM lst)
{
  InputAxis* axis = AxisFactory::create(gh_car(lst));
  bool top = gh_scm2bool(gh_cadr(lst));
  
  return new AxisButton(axis, top);
}

InputButton*
ButtonFactory::create_joystick_button(SCM lst)
{
  int device_num = gh_scm2int(gh_car(lst));
  int button_num = gh_scm2int(gh_cadr(lst));
  
  if (device_num >= 0 && device_num < CL_Joystick::get_device_count())
    return new InputButtonInputDevice(CL_Joystick::get_device(device_num), button_num);
  else
    {
      throw FeuerkraftError("Error: ButtonFactory::create_joystick_button: device out of range"
                            + CL_String::to(device_num) + " " + Guile::scm2string(lst));
    }
}

InputButton*
ButtonFactory::create_keyboard_button(SCM lst)
{
  std::string key_str = Guile::scm2string(gh_car(lst));
  int key_num         = CL_Keyboard::get_device().string_to_keyid(key_str);

  // FIXME: No error checking
  return new InputButtonInputDevice(CL_Keyboard::get_device(), key_num);
}

InputButton*
ButtonFactory::create_multi_button(SCM lst)
{
  MultiButton* button = new MultiButton();
  
  while (!gh_null_p(lst))
    {
      button->add(create(gh_car(lst)));
      lst = gh_cdr(lst);
    }
  
  return button;
}

/* EOF */
