/*
**  Windstille - A Sci-Fi Action-Adventure Game
**  Copyright (C) 2005 Ingo Ruhnke <grumbel@gmx.de>
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

#ifndef HEADER_WINDSTILLE_APP_CONSOLE_HPP
#define HEADER_WINDSTILLE_APP_CONSOLE_HPP

#include <boost/scoped_ptr.hpp>
#include <iosfwd>

#include "util/currenton.hpp"

class ConsoleImpl;

class Console : public Currenton<Console>
{
public:
  Console();
  ~Console();

  std::ostream& get_ostream() const;

  void draw();
  void update(float delta);

  void activate();
  void deactive();
  bool is_active() const;

  void scroll(int lines);

  void add(char* buf, int len);

  /** execute the given string */
  void execute(const std::string& str);

private:
  boost::scoped_ptr<ConsoleImpl> impl;

private:
  Console (const Console&);
  Console& operator= (const Console&);
};

#define ConsoleLog Console::current()->get_ostream()

#endif

/* EOF */
