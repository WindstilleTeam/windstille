/*  $Id$
**   __      __ __             ___        __   __ __   __
**  /  \    /  \__| ____    __| _/_______/  |_|__|  | |  |   ____
**  \   \/\/   /  |/    \  / __ |/  ___/\   __\  |  | |  | _/ __ \
**   \        /|  |   |  \/ /_/ |\___ \  |  | |  |  |_|  |_\  ___/
**    \__/\  / |__|___|  /\____ /____  > |__| |__|____/____/\___  >
**         \/          \/      \/    \/                         \/
**  Copyright (C) 2005 Ingo Ruhnke <grumbel@gmx.de>
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

#ifndef HEADER_SEGMENT_HPP
#define HEADER_SEGMENT_HPP

#include "math/line.hpp"
#include "math/vector.hpp"
#include "properties.hpp"

class Node;

/** */
class Segment
{
private:
  Node* node1;
  Node* node2;
  
  Properties props;

public:
  Segment(Node* node1_, Node* node2_, Properties props_ = 0);
  ~Segment();

  /** Calculate the angle between two segments */
  float angle(Segment* seg);
  
  Node* get_node1() const { return node1; } 
  Node* get_node2() const { return node2; } 

  Line get_line() const;
  Vector get_vector() const;
  
private:
  Segment (const Segment&);
  Segment& operator= (const Segment&);
};

#endif

/* EOF */
