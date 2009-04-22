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

#ifndef HEADER_WINDSTILLE_EDITOR_FUNCTOR_COMMAND_HPP
#define HEADER_WINDSTILLE_EDITOR_FUNCTOR_COMMAND_HPP

#include <boost/function.hpp>

#include "command.hpp"

class FunctorCommand : public Command
{
private:
  boost::function<void ()> undo_callback;
  boost::function<void ()> redo_callback;

public:
  FunctorCommand(const boost::function<void ()>& undo_callback_,
                 const boost::function<void ()>& redo_callback_)
    : undo_callback(undo_callback_),
      redo_callback(redo_callback_)
  {}

  void redo() {
    redo_callback();
  }

  void undo() {
    undo_callback();
  }

private:
  FunctorCommand(const FunctorCommand&);
  FunctorCommand& operator=(const FunctorCommand&);
};

#endif

/* EOF */
