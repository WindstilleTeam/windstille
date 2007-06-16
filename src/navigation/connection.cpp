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

#include "segment.hpp"
#include "node.hpp"
#include "display/display.hpp"
#include "connection.hpp"

Connection::Connection(Segment* segment_, float pos_)
  : segment(segment_),
    pos(pos_)
{  
}

void
Connection::set_pos(Segment* segment_, float pos_)
{
  segment = segment_;
  pos     = pos_;
}

void
Connection::advance(float& adv, Node*& next_node)
{
  Vector p1 = segment->get_node1()->get_pos();
  Vector p2 = segment->get_node2()->get_pos();
  
  float length = (p2 - p1).length();
  
  // convert from world co to [0,1] range
  float adv_01 = adv / length;

  if (adv_01 > 0)
    {
      pos += adv_01;
      if (pos > 1.0f) {
        adv = (pos - 1.0f) * length;
        pos = 1.0f;
        next_node = segment->get_node2();
      } else {
        adv = 0;
      }
    }
  else
    {
      pos += adv_01;
      if (pos < 0.0f) {
        adv = pos * length;
        pos = 0;
        next_node = segment->get_node1();
      } else {
        adv = 0;
      }
    }
}

void
Connection::advance(Vector& adv, Node*& next_node)
{
  // FIXME: This might be optimizable
  Vector p1 = segment->get_node1()->get_pos();
  Vector p2 = segment->get_node2()->get_pos();
  
  Vector segment_v = p2 - p1;

  Vector proj = adv.project(segment_v);

  float angle = atan2(segment_v.y, segment_v.x) - atan2(proj.y, proj.x);

  // Check if we are going forward or backward
  float advf;
  if (angle > M_PI/2 || angle < -M_PI/2)
    advf = -proj.length();
  else
    advf = proj.length();

  // Move forward
  advance(advf, next_node);
  
  // Calculate the rest Vector
  if (advf == 0.0f)
    adv = Vector(0,0);
  else
    adv -= (proj * ((proj.length() - advf)/proj.length()));
}

Vector
Connection::get_pos() const
{
  Vector p1 = segment->get_node1()->get_pos();
  Vector p2 = segment->get_node2()->get_pos();

  return p1 + pos*(p2 - p1);
}

void
Connection::draw()
{
  Display::fill_circle(get_pos(), 16.0f, Color(0.0f, 0.0f, 1.0f));
  Display::fill_circle(get_pos(), 8.0f, Color(0.0f, 1.0f, 1.0f));
}

/* EOF */
