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

#ifndef HEADER_AXIS_FACTORY_HXX
#define HEADER_AXIS_FACTORY_HXX

#include <guile/gh.h>
#include "input_axis.hxx"

/** */
class AxisFactory
{
public:
  static InputAxis* create(SCM lst);
private:
  static InputAxis* create_joystick_axis(SCM lst);
  static InputAxis* create_button_axis(SCM lst);
};

#endif

/* EOF */
