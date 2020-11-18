//  $Id: windstille_level.cxx,v 1.14 2003/11/13 12:59:42 grumbel Exp $
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

#include <assert.h>
#include <iostream>
#include "globals.hxx"
#include "windstille_level.hxx"
#include "string_converter.hxx"
#include "scm_helper.hxx"

WindstilleLevel::WindstilleLevel (const std::string& filename)
  : tilemap(0),
    background_tilemap(0)
{
  width  = 50;
  height = 50;
  parse_file (filename);
}

void 
WindstilleLevel::parse_file (const std::string& filename)
{
  diamond_map = 0;

  if (debug)
    std::cout << "Windstille Level: " << filename << std::endl;
  
  SCM input_stream = scm_open_file(scm_from_utf8_string(filename.c_str()), 
                                   scm_from_utf8_string("r"));
  SCM tree = scm_read(input_stream);
  
  if (!(scm_is_true(scm_symbol_p(scm_car(tree))) && scm_is_true(scm_equal_p(scm_from_utf8_symbol("windstille-level"), scm_car(tree)))))
    {
      std::cout << filename << ": not a Windstille Level file!" << std::endl;
    }
  else
    {
      tree = scm_cdr(tree);

      while (!scm_is_true(scm_null_p(tree)))
        {
          SCM current = scm_car(tree);
          if (scm_is_true(scm_pair_p(current)))
            {
              SCM name    = scm_car(current);
              SCM data    = scm_cdr(current);
      
              if (scm_is_true(scm_equal_p(scm_from_utf8_symbol("tilemap"), name)))
                {
                  parse_foreground_tilemap(data);
                }
              else if (scm_is_true(scm_equal_p(scm_from_utf8_symbol("background-tilemap"), name)))
                {
                  parse_background_tilemap(data);
                }
              else if (scm_is_true(scm_equal_p(scm_from_utf8_symbol("water"), name)))
                {
                  parse_water(data);
                }
              else if (scm_is_true(scm_equal_p(scm_from_utf8_symbol("properties"), name)))
                {
                  parse_properties(data);
                }
              else if (scm_is_true(scm_equal_p(scm_from_utf8_symbol("diamond-map"), name)))
                {
                  parse_diamond_map(data);
                }
              else if (scm_is_true(scm_equal_p(scm_from_utf8_symbol("scripts"), name)))
                {
                  parse_scripts(data);
                }
              else
                {
                  std::cout << "WindstilleLevel: Unknown tag: " << scm2string(name) << std::endl;
                }
            }
          else
            {
              std::cout << "WindstilleLevel: Not a pair!"  << std::endl;
            }
          tree = scm_cdr(tree);
        }
    }
  if (!diamond_map)
    {
      std::cout << "No diamond map in level file" << std::endl;
      diamond_map = new Field<int>(width * 2, height * 2);
    }
}

void
WindstilleLevel::parse_water(SCM tree)
{
  while (!scm_is_true(scm_null_p(tree)))
    {
      SCM current = scm_car(tree);

      if (scm_is_true(scm_pair_p(current)))
        {
          SCM name    = scm_car(current);
          SCM data    = scm_cdr(current);
      
          if (scm_is_true(scm_equal_p(scm_from_utf8_symbol("water"), name)))
            {
              //scm_display(data, SCM_UNDEFINED);
              //scm_newline();
              int x = scm_to_int(scm_car(data));
              int y = scm_to_int(scm_cadr(data));
              int w = scm_to_int(scm_caddr(data));
              int h = scm_to_int(scm_car(scm_cdddr(data)));
              std::cout << "Water: " << x << " " << y << " " << w << " " << h << std::endl;
            }

          tree = scm_cdr(tree);
        }
    }  
}

void
WindstilleLevel::parse_properties (SCM tree)
{
  while (!scm_is_true(scm_null_p(tree)))
    {
      SCM current = scm_car(tree);

      if (scm_is_true(scm_pair_p(current)))
        {
          SCM name    = scm_car(current);
          SCM data    = scm_cadr(current);
      
          if (scm_is_true(scm_equal_p(scm_from_utf8_symbol("width"), name)))
            {
              width  = scm_to_int(data);
            }
          else if (scm_is_true(scm_equal_p(scm_from_utf8_symbol("height"), name)))
            {
              height = scm_to_int(data);
            }
          else if (scm_is_true(scm_equal_p(scm_from_utf8_symbol("name"), name)))
            {
            }
          else
            {
              char* str = scm_to_utf8_string(name);
              std::cout << "WindstilleLevel::parse_properties: Unknown tag: "
                        << str
                        << std::endl;
              free(str);
            }

          tree = scm_cdr(tree);
        }
    }

  if (debug)
    std::cout << "WindstilleLevel: dimensions: " << width << "x" << height << std::endl;
}

void
WindstilleLevel::parse_background_tilemap (SCM cur)
{
  background_tilemap = parse_tilemap(cur);
}

void
WindstilleLevel::parse_foreground_tilemap (SCM cur)
{
  tilemap = parse_tilemap(cur);
}

Field<int>* 
WindstilleLevel::parse_tilemap (SCM cur)
{
  Field<int>* field = new Field<int>(width, height);
  
  int x = 0;
  int y = 0;
  while (!scm_is_true(scm_null_p(cur)) && y < height)
    {
      SCM name = scm_caar(cur);
      SCM data = scm_cdar(cur);
      
      if (scm_is_true(scm_equal_p(scm_from_utf8_symbol("data"), name)))
        {
          while (!scm_is_true(scm_null_p(data)) && y < height)
            {
              int id = scm_to_int(scm_car(data));
              (*field)(x, y) = id;
              
              x += 1;

              if (x >= width)
                {
                  x = 0;
                  y += 1;
                }
              
              data = scm_cdr(data);
            }
          if (y != height)
            std::cout << "WindstilleLevel: Something went wrong: y=" << y << " height=" << height << std::endl;
        }
          
      cur = scm_cdr(cur);
    }
  return field;
}

void
WindstilleLevel::parse_diamond_map(SCM data)
{
  diamond_map = new Field<int>(width * 2, height * 2);

  for(Field<int>::iterator i = diamond_map->begin(); i != diamond_map->end(); ++i)
    {
      *i = false;
    }
  
  int x = 0;
  int y = 0;

  while (!scm_is_true(scm_null_p(data)) && y < height*2)
    {
      (*diamond_map)(x, y) = scm_to_int(scm_car(data));
              
      x += 1;

      if (x >= width*2)
        {
          x = 0;
          y += 1;
        }
              
      data = scm_cdr(data);
    }

  if (y != height*2)
    std::cout << "WindstilleLevel: Something went wrong: y=" << y << " height=" << height << std::endl;
}

void
WindstilleLevel::parse_scripts(SCM data)
{
  while (!scm_is_true(scm_null_p(data)))
    {
      char* str = scm_to_utf8_string(scm_car(data));
      scripts.push_back(str);
      free(str);

      data = scm_cdr(data);
    }
}

void
WindstilleLevel::parse_gameobjects (SCM cur)
{
}

/* EOF */
