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

#include "math/easing.hpp"

#ifdef __TEST__
#include <iostream>

int main()
{
  for(float i = 0.0f; i < 1.0f; i += 0.01f)
  {
    std::cout << math::easing::circ::ease_in_out(i, 1.0f, 1.0f, 1.0f) << std::endl;
  }

  return 0;
}

#endif

/* EOF */
