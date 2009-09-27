//  $Id: input_manager.cxx,v 1.4 2003/08/20 00:15:10 grumbel Exp $
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
#include <assert.h>
#include <stdexcept>
#include <sstream>
#include <ClanLib/Display/joystick.h>

#include "../feuerkraft_error.hxx"
#include "input_manager_custom.hxx"
#include "input_manager_player.hxx"
#include "input_manager_impl.hxx"
#include "input_recorder.hxx"
#include "input_manager.hxx"

InputManagerImpl* InputManager::impl = 0;
InputRecorder* InputManager::recorder = 0;

void
InputManager::init_playback(const std::string& filename)
{
  impl = new InputManagerPlayer(filename);
}

void
InputManager::init(const std::string& filename)
{
  std::cout << "InputManager::init(" << filename << ")" << std::endl;
  SCM port = scm_open_file(gh_str02scm(filename.c_str()),
                           gh_str02scm("r"));
  SCM lst  = scm_read(port);

  if (gh_equal_p(gh_symbol2scm("feuerkraft-controller"), gh_car(lst)))
    {
      impl = new InputManagerCustom(gh_cdr(lst));
    }
  else
    {
      throw FeuerkraftError("Error: not a valid controller file: " + filename);
    }
  scm_close_port(port);
}

void
InputManager::setup_recorder(const std::string& filename)
{
  if (recorder)
    delete recorder;

  recorder = new InputRecorder(filename);
}

void 
InputManager::deinit()
{
  delete impl;
}

void
InputManager::update(float delta)
{
  assert(impl);
  impl->update(delta);
  if (recorder)
    recorder->record(get_controller());
}

Controller
InputManager::get_controller()
{
  assert(impl);
  return impl->get_controller();
}

void
InputManager::clear()
{
  impl->clear();
}

/* EOF */
