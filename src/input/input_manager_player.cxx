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

#include <iostream>
#include "../guile.hxx"
#include "input_manager_player.hxx"

InputManagerPlayer::InputManagerPlayer(const std::string& filename)
{
  std::cout << "InputManagerPlayer::InputManagerPlayer(" << filename << ")" << std::endl;
  entry_counter = 0;
  SCM port = scm_open_file(scm_from_utf8_string(filename.c_str()),
                           scm_from_utf8_string("r"));
  SCM entry;
  while(scm_eof_object_p(entry = scm_read(port)) == SCM_BOOL_F)
    {
      InputEventLst lst;
      int entry_num = scm_to_int(scm_cadr(entry));
      entry = scm_cddr(entry);
      
      while(scm_pair_p(entry))
        {
          lst.push_back(scm2event(scm_car(entry)));
          entry = scm_cdr(entry);
        }
      entries.push(Entry(entry_num, lst));
    }
  scm_close_port(port);
}

InputEvent
InputManagerPlayer::scm2event(SCM entry)
{
  InputEvent event;
  SCM sym  = scm_car(entry);
  SCM data = scm_cdr(entry);

  if (scm_equal_p(scm_from_utf8_symbol("axis"), sym)) 
    {
      event.type = AXIS_EVENT;
      event.axis.name = scm_to_int(scm_car(data));
      event.axis.pos  = scm_to_double(scm_cadr(data));
    } 
  else if (scm_equal_p(scm_from_utf8_symbol("button"), sym))
    {
      event.type = BUTTON_EVENT;
      event.button.name = scm_to_int(scm_car(data));
      event.button.down = scm_to_int(scm_cadr(data));
    } 
  else 
    {
      std::cout << "scm2event: Unknown sym: " << Guile::scm2string(sym) << std::endl;
    }
  return event;
}
  
void
InputManagerPlayer::update(float delta)
{
  if (entries.front().entry_num == entry_counter)
    {
      events = entries.front().events;
      
      for (InputEventLst::iterator i = events.begin(); i != events.end(); ++i)
        {
          if (i->type == AXIS_EVENT)
            controller.set_axis_state(i->axis.name, i->axis.pos);
          else if  (i->type == BUTTON_EVENT)
            controller.set_button_state(i->button.name, i->button.down);
        }
      entries.pop();
    }

  entry_counter += 1;
}

/* EOF */
