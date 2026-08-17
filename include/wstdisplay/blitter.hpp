// Windstille Display Library
// Copyright (C) 2002-2020 Ingo Ruhnke <grumbel@gmail.com>
//
// This program is free software: you can redistribute it and/or modify it
// under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation, either version 3 of the License, or (at your
// option) any later version.
//
// This program is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
// or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public
// License for more details.
//
// You should have received a copy of the GNU Lesser General Public License
// along with this program. If not, see <http://www.gnu.org/licenses/>.

#ifndef HEADER_WINDSTILLE_DISPLAY_BLITTER_HPP
#define HEADER_WINDSTILLE_DISPLAY_BLITTER_HPP

#include "software_surface.hpp"

namespace wstdisplay {

/** Duplicate all the edge pixel of the given rectangle to the outside
    of the rectangle, thus creating a border around the given
    rectangle, this is needed for OpenGL textures to avoid filtering
    artefacts:

    X X X X X    1 1 2 3 3
    X 1 2 3 X _\ 1 1 2 3 3
    X 4 5 6 X  / 4 4 5 6 6
    X X X X X    4 4 5 6 6
*/
void generate_border(SoftwareSurface& surface, int x_pos, int y_pos, int width, int height);

} // namespace wstdisplay

#endif

/* EOF */
