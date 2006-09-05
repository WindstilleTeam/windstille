/*  $Id$
**   __      __ __             ___        __   __ __   __
**  /  \    /  \__| ____    __| _/_______/  |_|__|  | |  |   ____
**  \   \/\/   /  |/    \  / __ |/  ___/\   __\  |  | |  | _/ __ \
**   \        /|  |   |  \/ /_/ |\___ \  |  | |  |  |_|  |_\  ___/
**    \__/\  / |__|___|  /\____ /____  > |__| |__|____/____/\___  >
**         \/          \/      \/    \/                         \/
**  Copyright (C) 2000,2005 Matthias Braun <matze@braunis.de>
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

#include <config.h>

#include "lisp/lisp.hpp"
#include "lisp/getters.hpp"
#include "lisp_getters.hpp"
#include "math/vector.hpp"
#include "color.hpp"

namespace lisp
{

bool property_get(const lisp::Lisp* lisp, Color& color)
{
  std::vector<float> col;
  if(property_get(lisp, col) == false)
    return false;
  if(col.size() == 3)
    color = Color(col[0], col[1], col[2]);
  else if(col.size() == 4)
    color = Color(col[0], col[1], col[2], col[3]);
  else
    return false;
  
  return true;
}

bool property_get(const lisp::Lisp* lisp, Vector& vec)
{
  size_t size = lisp->get_list_size();
  if(size != 4 && size != 3)
    return false;
  if(get(lisp->get_list_elem(1), vec.x) == false)
    return false;
  if(get(lisp->get_list_elem(2), vec.y) == false)
    return false;
  return true;
}

}

/* EOF */
