//  $Id: input_manager.hxx,v 1.3 2003/06/06 18:36:24 grumbel Exp $
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

#ifndef HEADER_INPUT_MANAGER_HXX
#define HEADER_INPUT_MANAGER_HXX

#include <vector>
#include "controller.hxx"
#include "input_event.hxx"

class InputRecorder;
class InputManagerImpl;

/** */
class InputManager
{
private:
  static InputManagerImpl* impl;
  static InputRecorder* recorder;
public:
  /** Init the InputManager with the data found in \a filename */
  static void init(const std::string& filename = std::string());

  /** Init the playback of a previously recorded file */
  static void init_playback(const std::string& filenam);
  static void deinit();

  /** Record all input events to \a filename */
  static void setup_recorder(const std::string& filename);

  static void update(float delta);
  static Controller get_controller();
  static void clear();
private:
  InputManager(const InputManager&);
  InputManager& operator=(const InputManager&);
};

#endif

/* EOF */
