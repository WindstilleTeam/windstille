/*  $Id$
**   __      __ __             ___        __   __ __   __
**  /  \    /  \__| ____    __| _/_______/  |_|__|  | |  |   ____
**  \   \/\/   /  |/    \  / __ |/  ___/\   __\  |  | |  | _/ __ \
**   \        /|  |   |  \/ /_/ |\___ \  |  | |  |  |_|  |_\  ___/
**    \__/\  / |__|___|  /\____ /____  > |__| |__|____/____/\___  >
**         \/          \/      \/    \/                         \/
**  Copyright (C) 2000,2005 Ingo Ruhnke <grumbel@gmx.de>
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

#ifndef HEADER_RANDOMIZER_HXX
#define HEADER_RANDOMIZER_HXX

#include "random.hpp"

class Randomizer 
{
public:
  virtual ~Randomizer() {}
  virtual void set_pos(Particle& p) =0;
};

class PointRandomizer : public Randomizer {
public:
  void set_pos(Particle& particle) {
    particle.x = 0;
    particle.y = 0;
  }
};

class RectRandomizer : public Randomizer {
public:
  Rectf rect;

  RectRandomizer(const Rectf& rect_)
    : rect(rect_) {}
 
  void set_pos(Particle& p) {
    p.x = rnd.drand(rect.left, rect.right);
    p.y = rnd.drand(rect.top,  rect.bottom);
  }
};

class CircleRandomizer : public Randomizer  {
public:
  float radius;

  CircleRandomizer(float radius_) 
    : radius(radius_) {}

  void set_pos(Particle& p) {
    // FIXME: BROKEN!!!!!
    p.x = rnd.drand(-radius, radius);
    p.y = sqrt((radius*radius) - (p.x*p.x)) * rnd.drand(-1.0f, 1.0f);
  }
};

class LineRandomizer : public Randomizer {
public:
  float x1, y1;
  float x2, y2;

  LineRandomizer(float x1_, float y1_, float x2_, float y2_) 
    : x1(x1_), y1(y1_), x2(x2_), y2(y2_)
  {}

  void set_pos(Particle& p) {
    float l = rnd.drand();
    p.x = x1 + (x2-x1) * l;
    p.y = y1 + (y2-y1) * l;
  }
};

#endif

/* EOF */
