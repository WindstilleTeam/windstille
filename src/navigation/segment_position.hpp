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

#ifndef HEADER_CONNECTION_HPP
#define HEADER_CONNECTION_HPP

#include "math/vector.hpp"

class Segment;
class Node;

/** 
 */
class SegmentPosition
{
public:
  Segment* segment;

  /** Position on the segment, stored with range [0,1], not
      world-co */
  float pos;

public:
  SegmentPosition();
  SegmentPosition(Segment* segment_, float pos_);

  void set_pos(Segment* segment_, float pos_);

  /** Move forward \a adv of units in world-co, when a node is hit,
   *  the function returns and let the user decide how to continue
   *
   *  @param[in,out] adv the amount of advancment to be done, the
   *                     amount of units that wheren't use on the
   *                     given segment
   *
   *  @param[out] next_node if the advance ends at a node, it gets
   *                        returned in next_node
   */
  void advance(float& adv, Node*& next_node);
  
  /** Move forward \a adv of units in world-co, when a node is hit,
   *  the function returns and let the user decide how to
   *  continue. \adv is projected onto the current segment to figure
   *  out how far we should go
   *
   * @param[in,out] adv the amount of advancment to be done, the
   *                    amount of units that wheren't use on the given
   *                    segment
   *
   * @param[out] next_node if the advance ends at a node, it gets
   *                       returned in next_node
   */  
  void advance(Vector& adv, Node*& next_node);

  Segment* get_segment() const { return segment; }
  float    get_float_pos() const { return pos; }

  Vector get_pos() const;

  void draw();
};

#endif

/* EOF */
