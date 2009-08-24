/*
**  Windstille - A Sci-Fi Action-Adventure Game
**  Copyright (C) 2009 Ingo Ruhnke <grumbel@gmx.de>
**
**  This program is free software: you can redistribute it and/or modify
**  it under the terms of the GNU General Public License as published by
**  the Free Software Foundation, either version 3 of the License, or
**  (at your option) any later version.
**  
**  This program is distributed in the hope that it will be useful,
**  but WITHOUT ANY WARRANTY; without even the implied warranty of
**  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
**  GNU General Public License for more details.
**  
**  You should have received a copy of the GNU General Public License
**  along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "undo_manager.hpp"

UndoManager::UndoManager()
  : undo_stack(),
    redo_stack()
{
}

UndoManager::~UndoManager()
{
}

void
UndoManager::execute(CommandHandle cmd)
{
  cmd->redo();
  undo_stack.push_back(cmd);
}

void
UndoManager::undo()
{
  if (!undo_stack.empty())
    {
      CommandHandle cmd = undo_stack.back();
      undo_stack.pop_back();
      cmd->undo();
      redo_stack.push_back(cmd);
    }
}

void
UndoManager::redo()
{
  if (!redo_stack.empty())
    {
      CommandHandle cmd = redo_stack.back();
      redo_stack.pop_back();
      cmd->redo();
      undo_stack.push_back(cmd);
    }
}

/* EOF */
