/*
**  Windstille - A Sci-Fi Action-Adventure Game
**  Copyright (C) 2005 Matthias Braun <matze@braunis.de>
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

#ifndef HEADER_WINDSTILLE_SCRIPTING_SQUIRREL_ERROR_HPP
#define HEADER_WINDSTILLE_SCRIPTING_SQUIRREL_ERROR_HPP

#include <squirrel.h>
#include <stdexcept>

class Pathname;

namespace Scripting
{

/** Exception class for squirrel errors, it takes a squirrelvm and uses
 * sq_geterror() to retrieve additional information about the last error that
 * occured and creates a readable message from that.
 */
class SquirrelError : public std::exception
{
public:
  SquirrelError(HSQUIRRELVM v, const Pathname& path, const std::string& message) throw();
  SquirrelError(HSQUIRRELVM v, const std::string& context, const std::string& message) throw();
  SquirrelError(HSQUIRRELVM v, const std::string& message) throw();
  virtual ~SquirrelError() throw();

  const char* what() const throw();

private:
  std::string message;
};

} // namespace Scripting

#endif

/* EOF */
