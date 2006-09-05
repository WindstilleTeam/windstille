/*  $Id$
**   __      __ __             ___        __   __ __   __
**  /  \    /  \__| ____    __| _/_______/  |_|__|  | |  |   ____
**  \   \/\/   /  |/    \  / __ |/  ___/\   __\  |  | |  | _/ __ \
**   \        /|  |   |  \/ /_/ |\___ \  |  | |  |  |_|  |_\  ___/
**    \__/\  / |__|___|  /\____ /____  > |__| |__|____/____/\___  >
**         \/          \/      \/    \/                         \/
**  Copyright (C) 2005 Ingo Ruhnke <grumbel@gmx.de>
**
**  This program is free software; you can redistribute it and/or
**  modify it under the terms of the GNU General Public License
**  as published by the Free Software Foundation; either version 2
**  of the License, or (at your option) any later version.
**
**  This program is distributed in the hope that it will be useful,
**  but WITHOUT ANY WARRANTY; without even the implied warranty of
**  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
**  GNU General Public License for more details.
** 
**  You should have received a copy of the GNU General Public License
**  along with this program; if not, write to the Free Software
**  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA
**  02111-1307, USA.
*/

#ifndef HEADER_PDA_HPP
#define HEADER_PDA_HPP

#include "screen.hpp"

class TextArea;

class DialogEntry
{
public:
  DialogEntry(const std::string& arg_character, const std::string& arg_text);
  
  std::string character;
  std::string text;
};

class ObjectiveEntry
{
public:
  ObjectiveEntry(const std::string& arg_name, const std::string& arg_text);
  
  std::string name;
  std::string text;
  bool complete;
};

/** */
class PDA : public Screen
{
public:
  PDA();

  void draw();
  void update(float delta, const Controller& controller);
  void add_dialog(const std::string& character, const std::string& text);
  
  void add_objective(const std::string& name, const std::string& text);
  void objective_complete(const std::string& name);
  bool is_objective_given(const std::string& name);
  bool is_objective_complete(const std::string& name);

private:
  void show_objectives();
  void show_dialogs();
  
  TextArea* text_area;
  std::vector<DialogEntry> dialogs;
  std::vector<ObjectiveEntry> objectives;   
  
  std::string new_text;  
  std::string old_text;
  enum pda_state { PDA_OBJECTIVES, PDA_DIALOGS } state;
};

#endif

/* EOF */
