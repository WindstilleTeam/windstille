//  $Id: guile_gameobj.cxx,v 1.4 2003/08/19 13:49:37 grumbel Exp $
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

#include <iostream>
#include "scmconverter.hxx"
//#include "gameobj_smob.hxx"
#include "guile_gameobj.hxx"

GuileGameObj::GuileGameObj (SCM arg_obj, SCM arg_update, SCM arg_draw)
  : obj (arg_obj), scm_update (arg_update), scm_draw (arg_draw)
{
  scm_gc_protect_object (scm_update);
  scm_gc_protect_object (scm_draw);
  scm_gc_protect_object (obj);
  
  //smob = create_smob<GameObj> (this);
  scm_gc_protect_object (smob);
}

GuileGameObj::~GuileGameObj ()
{
  
}

void 
GuileGameObj::draw ()
{
  scm_call_1(scm_draw, smob);
}

void
GuileGameObj::update (float delta)
{
  //std::cout << "Calling update" << std::endl;
  scm_call_2(scm_update, smob, scm_from_double (delta));
  //std::cout << "Calling update:done" << std::endl;
}

SCM
GuileGameObj::set_data (SCM arg_obj)
{
  scm_gc_protect_object(arg_obj);
  obj = arg_obj;
  return arg_obj;
}

/* EOF */
