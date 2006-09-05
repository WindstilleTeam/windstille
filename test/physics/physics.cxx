//  $Id$
//
//  Pingus - A free Lemmings clone
//  Copyright (C) 2002 Ingo Ruhnke <grumbel@gmx.de>
//
//  This program is free software; you can redistribute it and/or
//  modify it under the terms of the GNU General Public License
//  as published by the Free Software Foundation; either version 2
//  of the License, or (at your option) any later version.
//
//  This program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
//
//  You should have received a copy of the GNU General Public License
//  along with this program; if not, write to the Free Software
//  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.

#include <iostream>
#include <assert.h>
#include <ClanLib/display.h>
#include "physics.hxx"

Physics::Physics()
{
  friction       = 0.01f;
  
  x_acceleration = 0.0f;
  y_acceleration = 5.0f;

  unstuck_velocity = 50.0f;
  minimum_velocity = .1f;
}

Physics::~Physics()
{
}

void
Physics::draw()
{
  for(Objects::iterator i = objects.begin(); i != objects.end(); ++i)
    {
      if (!i->collision || !i->movable)
        CL_Display::fill_rect(CL_Rectf(CL_Pointf(i->x_pos, i->y_pos),
                                       CL_Sizef(i->width, i->height)),
                              CL_Color(255, 255, 255));        
      else
        CL_Display::fill_rect(CL_Rectf(CL_Pointf(i->x_pos, i->y_pos),
                                       CL_Sizef(i->width, i->height)),
                              CL_Color(255, 0, 255));        

      CL_Display::draw_rect(CL_Rectf(CL_Pointf(i->x_pos, i->y_pos),
                                     CL_Sizef(i->width, i->height)),
                            CL_Color(155, 155, 155));        
      
      CL_Display::draw_line(i->x_pos + i->width/2,
                            i->y_pos + i->height/2,
                            i->x_pos + i->width/2 + i->x_velocity,
                            i->y_pos + i->height/2 + i->y_velocity,
                            CL_Color(255, 0, 255));
    }
}

Physics::CollisionResult
Physics::simplesweep1d(float a, float aw, float av,
                       float b, float bw, float bv)
{
  // Normalize the calculation so that only A moves and B stands still
  float v = av - bv;

  if (v > 0)
    {
      CollisionResult res;
      res.u0 = (b - (a + aw)) / v;
      res.u1 = (b + bw - a) / v;
      res.state = COL_AT;

      assert(res.u0 <= res.u1);
      return res;
    }
  else if (v < 0)
    {
      CollisionResult res;
      res.u0 = (b + bw - a) / v;
      res.u1 = (b - (a + aw)) / v;
      res.state = COL_AT;

      assert(res.u0 <= res.u1);
      return res;
    }
  else // (v == 0)
    {
      CollisionResult res;

      if ((a + aw) < b || (a > b + bw))
        res.state = COL_NEVER;
      else
        res.state = COL_ALWAYS;

      return res;
    }
}

void
Physics::collision(PhysicObject& a, PhysicObject& b, Side side)
{
  switch(side)
    {
    case LEFT:
      if (!a.movable)
        {
          b.x_velocity = -b.x_velocity * 0.7f;
        }
      else if (!b.movable)
        {
          a.x_velocity = -a.x_velocity * 0.7f;
        }
      else
        {
          a.x_velocity = b.x_velocity * 0.7f;
          b.x_velocity = a.x_velocity * 0.7f;
        }
      break;

    case TOP:
      if (!a.movable)
        {
          b.y_velocity = -b.y_velocity * 0.7f;
        }
      else if (!b.movable)
        {
          a.y_velocity = -a.y_velocity * 0.7f;
        }
      else
        {
          a.y_velocity = b.y_velocity * 0.7f;
          b.y_velocity = a.y_velocity * 0.7f;
        }
      break;
    }
}

void
Physics::unstuck(PhysicObject& a, PhysicObject& b, float delta)
{
  // The distance A needs to unstuck from B in the given direction
  float left   = fabsf(a.x_pos + a.width - b.x_pos);
  float right  = fabsf(b.x_pos + b.width - a.x_pos);
  float top    = fabsf(a.y_pos + a.height - b.y_pos);
  float bottom = fabsf(b.y_pos + b.height - a.y_pos);

  float grace =  1.0f;

  if (left < right && left < top && left < bottom)
    {
      if (a.movable)
        a.x_pos -= std::min(left/2 + grace, unstuck_velocity * delta);
      
      if (b.movable)
        b.x_pos += std::min(left/2 + grace, unstuck_velocity * delta);
    }
  else if (right < left && right < top && right < bottom)
    {
      if (a.movable)
        a.x_pos += std::min(right/2 + grace, unstuck_velocity * delta);

      if (b.movable)
        b.x_pos -= std::min(right/2 + grace, unstuck_velocity * delta);
    }
  else if (top < left && top < right && top < bottom)
    {
      if (a.movable)
        a.y_pos -= std::min(top/2 + grace, unstuck_velocity * delta);
      
      if (b.movable)
        b.y_pos += std::min(top/2 + grace, unstuck_velocity * delta);
    }
  else // (bottom < left && bottom < right && bottom < top)
    {
      if (a.movable)
        a.y_pos += std::min(bottom/2 + grace, unstuck_velocity * delta);
      
      if (b.movable)
        b.y_pos -= std::min(bottom/2 + grace, unstuck_velocity * delta);
    }
}
  
void
Physics::resolve_collision(PhysicObject& a, PhysicObject& b, CollisionResult& x, CollisionResult& y, 
                           float delta)
{
  if (x.state == COL_NEVER || y.state == COL_NEVER)
    {
      // nothing todo
    }
  else 
    {
      if (x.state == COL_AT)
        {
          if (x.u1 < 0 || x.u0 > delta)
            {
              // miss, no collision in the current time frame
            }
          else
            {
              if (y.state == COL_ALWAYS)
                {
                  a.collision = true;
                  if (y.u0 < 0 || x.u0 < 0) 
                    unstuck(a, b, delta);
                  collision(a, b, LEFT);
                }
              else if (y.state == COL_AT)
                {
                  if (y.u1 < 0 || y.u0 > delta)
                    {
                      // miss, no collision in the current time frame
                    }
                  else
                    {
                      a.collision = true;

                      if (y.u0 < 0 || x.u0 < 0) 
                        unstuck(a, b, delta);                        

                      if (y.u0 > x.u0)
                        collision(a, b, TOP);
                      else
                        collision(a, b, LEFT);
                    }
                }
              else
                {
                  assert(!"Never reached");
                }
            }
        }
      else if (x.state == COL_ALWAYS)
        {
          if (y.state == COL_AT)
            {
              if (y.u1 < 0 || y.u0 > delta)
                {
                  // miss, no collision in the current time frame
                }
              else
                {
                  if (y.u0 < 0 || x.u0 < 0) 
                    unstuck(a, b, delta);

                  // collision
                  a.collision = true;
                  collision(a, b, TOP);
                }
            }
          else if (y.state == COL_ALWAYS)
            {
              // stuck
              a.collision = true;
              collision(a, b, STUCK);
              unstuck(a, b, delta);
            }
          else
            {
              assert(!"Never reached");
            }
        }
      else
        {
          assert(!"Never reached");
        }
    }
}

void
Physics::update(float delta)
{
  for(Objects::iterator i = objects.begin(); i != objects.end(); ++i)
    {
      i->collision = false;
                
      //                        i - why doesn't this work?
      for(Objects::iterator j = i+1/*objects.begin()*/; j != objects.end(); ++j)
        {
          if (i != j)
            {
              CollisionResult x_res = simplesweep1d(i->x_pos, i->width, i->x_velocity,
                                                    j->x_pos, j->width, j->x_velocity);
              CollisionResult y_res = simplesweep1d(i->y_pos, i->height, i->y_velocity,
                                                    j->y_pos, j->height, j->y_velocity);
              resolve_collision(*i, *j, x_res, y_res, delta);
            }
        }
      
      if (i->movable)
        update(*i, delta);
    }
}

void
Physics::update(PhysicObject& obj, float delta)
{
  // Insert Collisions handling
  obj.x_pos += obj.x_velocity * delta;
  obj.y_pos += obj.y_velocity * delta;

  obj.x_velocity -= (obj.x_velocity * friction) * delta;
  obj.y_velocity -= (obj.y_velocity * friction) * delta;
              
  obj.x_velocity += x_acceleration * delta;
  obj.y_velocity += y_acceleration * delta;

  if (fabsf(obj.x_velocity) < minimum_velocity)
    obj.x_velocity = 0.0f;

  if (fabsf(obj.y_velocity) < minimum_velocity)
    obj.y_velocity = 0.0f;
}

PhysicObject&
Physics::add_object(PhysicObject obj)
{
  objects.push_back(obj);

  objects.back().id = objects.size();

  return objects.back();
}

/* EOF */
