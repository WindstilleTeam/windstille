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

#ifndef HEADER_BUTTON_FACTORY_HXX
#define HEADER_BUTTON_FACTORY_HXX

#include <guile/gh.h>

class InputButton;

/** */
class ButtonFactory
{
private:
public:
  static InputButton* create(SCM lst);

private:
  static InputButton* create_joystick_button(SCM lst);
  static InputButton* create_keyboard_button(SCM lst);
  static InputButton* create_axis_button(SCM lst);
  static InputButton* create_multi_button(SCM lst);

  ButtonFactory (const ButtonFactory&);
  ButtonFactory& operator= (const ButtonFactory&);
};

#endif

/* EOF */
