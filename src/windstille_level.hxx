//  $Id: windstille_level.hxx,v 1.4 2003/08/12 08:24:41 grumbel Exp $
// 
//  Windstille - A Jump'n Shoot Game
//  Copyright (C) 2000 Ingo Ruhnke <grumbel@gmx.de>
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

#ifndef WINDSTILLELEVEL_HXX
#define WINDSTILLELEVEL_HXX

#include <guile/gh.h>
#include <string>
#include "field.hxx"

class WindstilleLevel
{
private:
  Field<int>* field;

public:
  WindstilleLevel (const std::string& filename);

  Field<int>* get_field () const { return field; }
private:
  void parse_file (const std::string& filename);
  void parse_properties (SCM cur);
  void parse_tilemap (SCM cur);
  void parse_gameobjects (SCM cur);
};

#endif

/* EOF */