/*  $Id$
**   __      __ __             ___        __   __ __   __
**  /  \    /  \__| ____    __| _/_______/  |_|__|  | |  |   ____
**  \   \/\/   /  |/    \  / __ |/  ___/\   __\  |  | |  | _/ __ \
**   \        /|  |   |  \/ /_/ |\___ \  |  | |  |  |_|  |_\  ___/
**    \__/\  / |__|___|  /\____ /____  > |__| |__|____/____/\___  >
**         \/          \/      \/    \/                         \/
**  Copyright (C) 2007 Ingo Ruhnke <grumbel@gmx.de>
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

#include <iostream>
#include <algorithm>
#include "node.hpp"

Node::Node(const Vector& pos_)
  : pos(pos_)
    // FIXME: Do something with id
{
  std::cout << "Creating node: " << this << std::endl;
}

Node::~Node()
{
  std::cout << "Destroying node: " << this << std::endl;
}

void
Node::add_segment(const SegmentPosition& position)
{
  segments.push_back(position);
}

void
Node::remove_segment(Segment* segment)
{
  for(Segments::iterator i = segments.begin(); i != segments.end(); ++i)
    {
      if (i->segment == segment)
        {
          segments.erase(i);
          return;
        }
    }
}

/* EOF */
