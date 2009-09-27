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

#ifndef HEADER_INPUT_MANAGER_PLAYER_HXX
#define HEADER_INPUT_MANAGER_PLAYER_HXX

#include <queue>
#include <string>
#include <guile/gh.h>
#include "input_manager_impl.hxx"

/** Playback class for events recorded my the InputRecorder */
class InputManagerPlayer : public InputManagerImpl
{
private:
  struct Entry {
    Entry(int num, const InputEventLst& lst) 
      : entry_num(num), events(lst)
    {}
    int entry_num;
    InputEventLst events;
  };

  int entry_counter;
  std::queue<Entry> entries;
public:
  InputManagerPlayer(const std::string& filename);
  
  void update(float delta);
private:
  InputEvent scm2event(SCM lst);

  InputManagerPlayer (const InputManagerPlayer&);
  InputManagerPlayer& operator= (const InputManagerPlayer&);
};

#endif

/* EOF */
