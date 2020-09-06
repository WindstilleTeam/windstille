/*
**  Windstille - A Sci-Fi Action-Adventure Game
**  Copyright (C) 2009 Ingo Ruhnke <grumbel@gmail.com>
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

#ifndef HEADER_WINDSTILLE_EDITOR_GROUP_COMMAND_HPP
#define HEADER_WINDSTILLE_EDITOR_GROUP_COMMAND_HPP

#include <vector>

#include "editor/command.hpp"

class GroupCommand : public Command
{
private:
  typedef std::vector<CommandHandle> Commands;
  Commands cmds;

public:
  GroupCommand() : cmds() {}

  void add(CommandHandle cmd) {
    cmds.push_back(cmd);
  }

  void redo() override {
    for(Commands::const_iterator i = cmds.begin(); i != cmds.end(); ++i)
      (*i)->redo();
  }

  void undo() override {
    for(Commands::const_reverse_iterator i = cmds.rbegin(); i != cmds.rend(); ++i)
      (*i)->undo();
  }

private:
  GroupCommand(const GroupCommand&);
  GroupCommand& operator=(const GroupCommand&);
};

#endif

/* EOF */
