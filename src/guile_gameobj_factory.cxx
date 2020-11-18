//  $Id: guile_gameobj_factory.cxx,v 1.3 2003/08/19 13:40:48 grumbel Exp $
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
#include "guile_gameobj_factory.hxx"

GuileGameObjFactory::Descr GuileGameObjFactory::descriptions;

void
GuileGameObjFactory::register_guile_bindings ()
{
  scm_c_define_gsubr("gameobj-factory:register", 2, 0, 0, reinterpret_cast<void*>(&GuileGameObjFactory::register_object));
}

GuileGameObj*
GuileGameObjFactory::create (const std::string& name)
{
  Descr::iterator i = descriptions.find (name);
  
  if (i == descriptions.end ())
    {
      assert (!"GuileObject not found");
      return 0;
    }
  else 
    {
      return new GuileGameObj(scm_call_0(i->second.scm_create),
                              i->second.scm_update,
                              i->second.scm_draw);
    }
}

SCM
GuileGameObjFactory::register_object(SCM name, SCM vec)
{
  char* name_c = scm_to_utf8_string(name);
  descriptions[std::string(name_c)] = GuileGameObjDesc (scm_vector_ref (vec, scm_from_int(0)),
                                                        scm_vector_ref (vec, scm_from_int(1)),
                                                        scm_vector_ref (vec, scm_from_int(2)));
  free(name_c);

  return SCM_UNSPECIFIED;
}

/* EOF */
