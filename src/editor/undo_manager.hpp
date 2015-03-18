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

#ifndef HEADER_WINDSTILLE_EDITOR_UNDO_MANAGER_HPP
#define HEADER_WINDSTILLE_EDITOR_UNDO_MANAGER_HPP

#include <vector>

#include "editor/command.hpp"

class UndoManager
{
private:
  std::vector<CommandHandle> undo_stack;
  std::vector<CommandHandle> redo_stack;

public:
  UndoManager();
  ~UndoManager();

  void execute(CommandHandle cmd);

  void undo();
  void redo();

  bool can_undo() const { return !undo_stack.empty(); }
  bool can_redo() const { return !redo_stack.empty(); }

private:
  UndoManager(const UndoManager&);
  UndoManager& operator=(const UndoManager&);
};

#endif

/* EOF */
