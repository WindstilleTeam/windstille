//  $Id: scm_helper.cxx,v 1.2 2003/08/12 08:24:41 grumbel Exp $
//
//  Windstille - A Jump'n Shoot Game
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

#include "scm_helper.hxx"

std::string scm2string(SCM str)
{
  if (scm_is_true(scm_string_p(str)))
    {
      char* cstr = scm_to_utf8_string(str);
      std::string cppstr = cstr;
      free(cstr);
      return cppstr;
    }
  else if (scm_is_true(scm_symbol_p(str)))
    {
      char* cstr = scm_to_utf8_string(str);
      std::string cppstr = cstr;
      free(cstr);
      return cppstr;
    }
  else 
    {
      return "#!not a string!#";
    }
}

/* EOF */
