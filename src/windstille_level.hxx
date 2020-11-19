//  $Id: windstille_level.hxx,v 1.9 2003/09/26 14:29:36 grumbel Exp $
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

#include <libguile.h>
#include <string>
#include "field.hxx"

/** FIXME: Move this to the guile side */
class WindstilleLevel
{
private:
  /** Width of the levels in tiles (128x128 pixels each) */
  int width;

  /** Height of the levels in tiles (128x128 pixels each) */
  int height;

  Field<int>* tilemap;
  Field<int>* background_tilemap;

  Field<int>* diamond_map;

  std::vector<std::string> scripts;

  void parse_file (const std::string& filename);
  void parse_properties (SCM cur);

  Field<int>* parse_tilemap (SCM cur);
  void parse_foreground_tilemap (SCM cur);
  void parse_background_tilemap (SCM cur);
  void parse_water(SCM cur);
  void parse_diamond_map(SCM cur);
  void parse_backgound_tilemap (SCM cur);
  void parse_gameobjects (SCM cur);
  void parse_scripts(SCM cur);
public:
  WindstilleLevel (const std::string& filename);

  Field<int>* get_tilemap() const { return tilemap; }
  Field<int>* get_background_tilemap() const { return background_tilemap; }

  Field<int>* get_diamond_map() { return diamond_map; }

  std::vector<std::string>* get_scripts() { return &scripts; }
};

#endif

/* EOF */
