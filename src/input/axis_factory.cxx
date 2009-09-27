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
#include "input_axis_input_device.hxx"
#include "../feuerkraft_error.hxx"
#include "../guile.hxx"
#include "button_factory.hxx"
#include "button_axis.hxx"
#include "axis_factory.hxx"

InputAxis* 
AxisFactory::create(SCM lst)
{
  while(gh_pair_p(lst))
    {
      SCM sym  = gh_car(lst);
      SCM data = gh_cdr(lst);
      
      if (gh_equal_p(sym, gh_symbol2scm("joystick-axis")))
        {
          return create_joystick_axis(data);
        }
      if (gh_equal_p(sym, gh_symbol2scm("button-axis")))
        {
          return create_button_axis(data);
        }
      else
        {
          throw FeuerkraftError("AxisFactory::create: parse error");
        }

      lst = gh_cdr(lst);
    }
  return 0;
}

InputAxis*
AxisFactory::create_joystick_axis(SCM lst)
{
  int device_num = gh_scm2int(gh_car(lst));
  int axis_num   = gh_scm2int(gh_cadr(lst));

  if (device_num >= 0 && device_num < CL_Joystick::get_device_count())
    return new InputAxisInputDevice(CL_Joystick::get_device(device_num), axis_num);
  else
    {
      throw FeuerkraftError("Error: AxisFactory::create_joystick_axis: " 
                            + Guile::scm2string(lst));
      return 0;
    }
}

InputAxis*
AxisFactory::create_button_axis(SCM lst)
{
  InputButton* left  = ButtonFactory::create(gh_car(lst));
  InputButton* right = ButtonFactory::create(gh_cadr(lst));

  return new ButtonAxis(left, right);
}

/* EOF */
