/*
**  Windstille - A Sci-Fi Action-Adventure Game
**  Copyright (C) 2005 David Kamphausen <david.kamphausen@web.de>,
**                     Ingo Ruhnke <grumbel@gmail.com>
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

#include "collision/collision_engine.hpp"

#include <assert.h>

#include "collision/collision_test.hpp"
#include "tile/tile_map.hpp"

namespace windstille {

std::vector<geom::frect> tilemap_collision_list(TileMap *tilemap, geom::frect const& r, bool is_ground);

/***********************************************************************
 * Collision
 ***********************************************************************/

CollisionEngine::CollisionEngine()
  : objects(),
    unstuck_velocity()
{
  unstuck_velocity = 50.0f;
}

CollisionEngine::~CollisionEngine()
{
  for(Objects::iterator i = objects.begin(); i != objects.end(); ++i)
    delete *i;
  objects.clear();
}

void
CollisionEngine::draw(wstdisplay::DrawingContext& dc)
{
  for(Objects::iterator i = objects.begin(); i != objects.end(); ++i)
  {
    (*i)->draw(dc);
  }
}

void
CollisionEngine::collision(CollisionData const& result)
{
  assert(result.object1 && result.object2);

  if (result.object2->get_is_domains() & result.object1->get_check_domains())
  {
    result.object1->sig_collision()(result);
  }
  else
  {
    std::cout << (result.object2->get_is_domains() & result.object1->get_check_domains()) << std::endl;
  }

  unsigned int res1 = result.object1->get_is_domains() & result.object2->get_check_domains();

  if (res1)
  {
    result.object2->sig_collision()(result.invert());
  }
  else
  {
    std::cout << "obj1: " << result.object1->get_is_domains() << std::endl;
    std::cout << "obj2: " << result.object2->get_check_domains() << std::endl;
  }
}

void
CollisionEngine::unstuck(CollisionObject& a, CollisionObject& b, float delta)
{
  if (a.get_type() == CollisionObject::RECTANGLE && b.get_type() == CollisionObject::RECTANGLE)
  {
    unstuck_rect_rect (a, b, delta);
  }
  else
  {
    if (a.get_type() == CollisionObject::RECTANGLE)
      unstuck_tilemap (b, a, delta);
    else
      unstuck_tilemap (a, b, delta);

  }
}

namespace {

glm::vec2 unstuck_direction(geom::frect const& a, geom::frect const& b, float delta, float unstuck_velocity)
{
  // The distance A needs to unstuck from B in the given direction
  float left   = fabsf(a.right() - b.left());
  float right  = fabsf(b.right() - a.left());
  float top    = fabsf(a.bottom() - b.top());
  float bottom = fabsf(b.bottom() - a.top());

  float grace =  0.05f;

  //float add = unstuck_velocity * delta;
  //float add = 0.5f;
  float add = 50.0f;
  //  grace=0;

  glm::vec2 dir;

  if (left < right && left < top && left < bottom)
  {
    dir = glm::vec2(std::min(left/2 + grace, add), 0);
  }
  else if (right < left && right < top && right < bottom)
  {
    dir = glm::vec2(-std::min(right/2 + grace, add), 0);
  }
  else if (top < left && top < right && top < bottom)
  {
    dir = glm::vec2( 0, std::min(top/2 + grace, add));
  }
  else // (bottom < left && bottom < right && bottom < top)
  {
    dir = glm::vec2( 0, -std::min(bottom/2 + grace, add));
  }
  return dir;
}

int c_roundup(float f)
{
  int i = static_cast<int>(f);
  if(f > static_cast<float>(i))
    i++;
  return i;
}

int c_round(float f)
{
  int i = static_cast<int>(f);
  if(fabsf(f-static_cast<float>(i)) > 0.5f)
  {
    if(i>0)
      i++;
    else
      i--;
  }
  return i;
}

bool is_rect_free(TileMap *tilemap, int l, int t, int w,int h)
{
  int x,y;
  for (x=l; x<=l+w; x++)
    for (y=t; y<=t+h; y++)
    {
      if (tilemap->is_ground(static_cast<float>(x * TILE_SIZE), static_cast<float>(y * TILE_SIZE)))
        return false;
    }
  return true;
}

geom::frect get_next_free_rect(TileMap *tilemap, geom::frect const& r)
{
  int rx = c_round(r.left() / static_cast<float>(TILE_SIZE));
  int ry = c_round(std::min (r.top(), r.bottom())  / static_cast<float>(TILE_SIZE));

  float fw = r.right() - r.left();
  float fh = fabsf(r.bottom() - r.top());

  int rw = c_roundup (fw / static_cast<float>(TILE_SIZE));
  int rh = c_roundup (fh / static_cast<float>(TILE_SIZE));

  std::vector<geom::frect> rects;

  // find first set of free rectangle
  // simply iterate the rectangles around current position
  for(int d=1; d<20; d++) // not more than 20 steps
  {
    for(int i=-d; i<=d; i++)
    {
      if (is_rect_free(tilemap, i + rx, -d + ry, rw, rh))
        rects.emplace_back(geom::irect(i + rx, -d + ry, rw, rh));
      if (is_rect_free(tilemap, i + rx, d + ry, rw, rh))
        rects.emplace_back(geom::irect(i + rx, d + ry, rw, rh));

      if (is_rect_free(tilemap, -d + rx, i + ry, rw, rh))
        rects.emplace_back(geom::irect(-d + rx, i + ry, rw, rh));
      if (is_rect_free(tilemap,  d + rx, i + ry, rw, rh))
        rects.emplace_back(geom::irect(d  + rx, i + ry, rw, rh));
    }
    if (rects.size())
      break;
  }
  assert(rects.size());

  // find nearest rectangle in this set
  float distance=10000.0f;
  float dx,dy,d;
  geom::frect nr;
  for (std::vector<geom::frect>::iterator i = rects.begin(); i != rects.end(); ++i)
  {
    dx = i->left() - r.left() / static_cast<float>(TILE_SIZE);
    dy = i->top()  - r.top()  / static_cast<float>(TILE_SIZE);
    d = sqrtf( dx * dx + dy * dy );
    if (d < distance)
    {
      distance=d;
      nr=*i;
    }
  }

  nr = geom::frect(nr.left(),
             nr.top(),
             nr.right() + nr.left(),
             nr.bottom() + nr.top());

  return nr;
}

} // namespace

void
CollisionEngine::unstuck_tilemap(CollisionObject& a, CollisionObject& b, float delta)
{
  (void)delta;
  geom::frect rb = b.primitive;

  rb = geom::frect(rb.left() + b.get_pos().x,
             rb.top()  + b.get_pos().y,
             rb.right() + b.get_pos().x,
             rb.bottom() + b.get_pos().y);

  geom::frect target = get_next_free_rect(a.tilemap, rb);

  target = geom::frect(target.left() * static_cast<float>(TILE_SIZE),
                 target.top() * static_cast<float>(TILE_SIZE),
                 target.left() + (rb.right() - rb.left()),
                 target.top()  + (rb.top() - rb.bottom()));

  // align to grid, if coming from right or bottom

  if(target.top() < rb.top())
  {
    float v = static_cast<float>(c_roundup(target.bottom() / static_cast<float>(TILE_SIZE)))
      * static_cast<float>(TILE_SIZE) - target.bottom();

    target = geom::frect(target.left(),
                   target.top() + v,
                   target.right(),
                   target.bottom() + v);
  }
  if(target.left() < rb.left())
  {
    float v = static_cast<float>(c_roundup(target.right() / static_cast<float>(TILE_SIZE))) * static_cast<float>(TILE_SIZE) - target.right();

    target = geom::frect(target.left() + v,
                   target.top(),
                   target.right() + v,
                   target.bottom());
  }

  b.pos = glm::vec2(target.left() - b.primitive.left(), target.top() - b.primitive.top());
}

void
CollisionEngine::unstuck_rect_rect(CollisionObject& a, CollisionObject& b, float delta)
{
  geom::frect ra = a.primitive;

  ra = geom::frect(ra.left()   + a.get_pos().x,
             ra.top()    + a.get_pos().y,
             ra.right()  + a.get_pos().x,
             ra.bottom() + a.get_pos().y);

  geom::frect rb = b.primitive;

  rb = geom::frect(rb.left()   + b.get_pos().x,
             rb.top()    + b.get_pos().y,
             rb.right()  + b.get_pos().x,
             rb.bottom() + b.get_pos().y);

  glm::vec2 dir = unstuck_direction (ra, rb, delta, unstuck_velocity);

  if (a.unstuck_movable())
    a.pos -= dir;

  if (b.unstuck_movable())
    b.pos += dir;
}

void
CollisionEngine::update(float delta)
{
  if (objects.empty())
    return;

  CollisionData col_data;
  float frame=delta;
  float min_time;
  int max_tries=200;

  do
  {
    min_time=frame;

    for(Objects::iterator i = objects.begin(); i != objects.end(); ++i)
    {
      for(Objects::iterator j = i + 1; j != objects.end(); ++j)
      {
        if (i != j && (((*i)->get_is_domains() & (*j)->get_check_domains()) ||
                       ((*j)->get_is_domains() & (*i)->get_check_domains())))
        {
          CollisionData r = collide(**i, **j, frame);
          if(r.state!=CollisionData::NONE)
          {
            if (min_time > r.col_time && r.col_time>=0)
            {
              r.object1 = *i;
              r.object2 = *j;
              col_data = r;
              min_time = r.col_time;
              if (min_time > 0.0005f)
                min_time -= 0.0005f;
            }
          }
        }
      }
    }

    // move till first collision (or till end, when no collision occured)
    if(min_time>0)
    {
      for(Objects::iterator i = objects.begin(); i != objects.end(); ++i)
      {
        update(**i,min_time);
      }
    }
    // report collision
    if (min_time < frame)
    {
      collision (col_data);
    }

    frame-=min_time;
    min_time=0;

    // check tries
    --max_tries;
    if (max_tries == 0)
      std::cerr<<"Too much tries in collision detection"<<std::endl;

  }while (min_time < frame  && max_tries>0);

  //return; // uncomment, if you want no unstucking

  // check penetration and resolve
  bool penetration = true;
  int maxtries=15;
  while(penetration)
  {
    penetration=false;
    // FIXME: support this by some spatial container
    for(Objects::iterator i = objects.begin(); i != objects.end(); ++i)
    {
      if(!(*i)->unstuck())
        continue;

      for(Objects::iterator j = i+1; j != objects.end(); ++j)
      {
        if(!(*j)->unstuck())
          continue;

        if (i != j && ((*i)->unstuck_movable() || ((*j)->unstuck_movable())))
        {
          CollisionData r = collide(**i, **j, 0);
          if(r.state!=CollisionData::NONE)
          {
            penetration=true;
            unstuck(**i, **j, delta/3.0f);
          }
        }
      }
    }
    maxtries--;
    if(maxtries==0)
      break;
  }
}

void
CollisionEngine::update(CollisionObject& obj, float delta)
{
  obj.update(delta);
}

CollisionObject*
CollisionEngine::add(CollisionObject *obj)
{
  // FIXME: This might need commit_add/commit_remove stuff like in sector
  objects.push_back(obj);

  return objects.back();
}

void
CollisionEngine::remove(CollisionObject *obj)
{
  // FIXME: This might need commit_add/commit_remove stuff like in sector
  Objects::iterator i = std::find(objects.begin(), objects.end(), obj);
  if(i != objects.end())
    objects.erase(i);
}

// LEFT means b1 is left of b2
CollisionData
CollisionEngine::collide(geom::frect const& b1, geom::frect const& b2,
                         glm::vec2 const& b1_v, glm::vec2 const& b2_v,
                         float delta)
{
  SweepResult result0 = simple_sweep_1d(b1.left(), b1.width(),  b1_v.x,
                                        b2.left(), b2.width(),  b2_v.x);
  SweepResult result1 = simple_sweep_1d(b1.top(),  b1.height(), b1_v.y,
                                        b2.top(),  b2.height(), b2_v.y);

  CollisionData result;
  result.delta = delta;

  if(result0.collision(delta) && result1.collision(delta))
  {
    if(result0.always() && result1.always())
      result.state = CollisionData::STUCK;
    else
    {
      if(result0.begin(delta)<result1.begin(delta))
      {
        // x direction prior
        if(b1.left() < b2.left())
        {
          result.state=CollisionData::COLLISION;
          result.direction=glm::vec2(-1, 0);
        }
        else
        {
          result.state=CollisionData::COLLISION;
          result.direction=glm::vec2(1, 0);
        }
        result.col_time=result0.t0;
      }
      else
      {
        // x direction prior
        if(b1.top() < b2.top())
        {
          result.state=CollisionData::COLLISION;
          result.direction=glm::vec2(0, -1);
        }
        else
        {
          result.state=CollisionData::COLLISION;
          result.direction=glm::vec2(0, 1);
        }
        result.col_time=result1.t0;
      }
    }
  }
  return result;
}

CollisionData
CollisionEngine::collide(CollisionObject& a, CollisionObject& b, float delta)
{
  if (a.get_type() == CollisionObject::RECTANGLE && b.get_type() == CollisionObject::RECTANGLE)
  {
    geom::frect ra = a.primitive;
    geom::frect rb = b.primitive;

    ra = geom::frect(ra.left()   + a.get_pos().x,
               ra.top()    + a.get_pos().y,
               ra.right()  + a.get_pos().x,
               ra.bottom() + a.get_pos().y);

    rb = geom::frect(rb.left()   + b.get_pos().x,
               rb.top()    + b.get_pos().y,
               rb.right()  + b.get_pos().x,
               rb.bottom() + b.get_pos().y);

    return collide(ra, rb,
                   a.get_velocity(), b.get_velocity(),
                   delta);
  }
  else
  {
    if (a.get_type() == CollisionObject::RECTANGLE)
      return collide_tilemap (b, a, delta).invert();
    else
      return collide_tilemap (a, b, delta);
  }
}

namespace {

int get_next_integer(float f, float direction)
{
  int result;

  if(direction < 0)
  {
    result = int (f);
    if (static_cast<float>(result) >= f)
      --result;
  }
  else
  {
    result = int (f);
    if (static_cast<float>(result) <= f)
      ++result;
  }
  return result;
}

int get_integer(float f, float direction)
{
  int result=static_cast<int>(f);

  if(direction < 0)
  {
    result = static_cast<int>(f);
    if (static_cast<float>(result) > f)
      --result;
  }
  return result;
}

bool tilemap_collision(TileMap *tilemap, geom::frect const& r)
{
  int minx, maxx;
  int miny, maxy;
  int x, y;

  minx = static_cast<int>(r.left()   / static_cast<float>(TILE_SIZE));
  maxx = static_cast<int>(r.right()  / static_cast<float>(TILE_SIZE));
  miny = static_cast<int>(r.top()    / static_cast<float>(TILE_SIZE));
  maxy = static_cast<int>(r.bottom() / static_cast<float>(TILE_SIZE));

  assert(maxy>=miny);
  assert(maxx>=minx);

  for (y = std::max (0, miny); y <= std::min (maxy, tilemap->get_height() - 1); ++y)
    for (x = std::max (0, minx); x <= std::min (maxx, tilemap->get_width() - 1); ++x)
    {
      if(tilemap->is_ground (static_cast<float>(x * TILE_SIZE), static_cast<float>(y * TILE_SIZE) ))
      {
        return true;
      }
    }
  return false;
}

#if 0
std::vector<geom::frect> tilemap_collision_list(TileMap *tilemap, geom::frect const& r,bool is_ground)
{
  std::vector<geom::frect> rect_list;
  int minx, maxx;
  int miny, maxy;
  int x, y;

  minx = static_cast<int>(r.left   / static_cast<float>(TILE_SIZE));
  maxx = static_cast<int>(r.right  / static_cast<float>(TILE_SIZE));
  miny = static_cast<int>(r.top    / static_cast<float>(TILE_SIZE));
  maxy = static_cast<int>(r.bottom / static_cast<float>(TILE_SIZE));

  for (y = std::max (0, miny); y <= std::min (maxy, tilemap->get_height() - 1); ++y)
    for (x = std::max (0, minx); x <= std::min (maxx, tilemap->get_width() - 1); ++x)
    {
      if(tilemap->is_ground (static_cast<float>(x * TILE_SIZE), static_cast<float>(y * TILE_SIZE) ) == is_ground)
      {
        rect_list.push_back (geom::frect (static_cast<float>(x * TILE_SIZE),
                                    static_cast<float>(y * TILE_SIZE),
                                    static_cast<float>(TILE_SIZE),
                                    static_cast<float>(TILE_SIZE)));
      }
    }
  return rect_list;
}
#endif

} // namespace

#define c_sign(x) ((x)<0?-1:((x)>0?1:0))

CollisionData
CollisionEngine::collide_tilemap(CollisionObject& a, CollisionObject& b, float delta)
{
  CollisionData result;

  assert(a.get_type() == CollisionObject::TILEMAP);
  assert(b.get_type() == CollisionObject::RECTANGLE);

  glm::vec2 const vel = b.get_velocity() - a.get_velocity();

  if (vel.x == 0.0f && vel.y == 0.0f)
    return result;

  // The algorithm functions under following assumption, that
  // b is in the free at time == 0.
  // Then for the given frame delta, for each new grid collision is checked,
  // if a collision takes place.

  geom::frect r = b.primitive;

  r = geom::frect(r.left()   + b.get_pos().x,
            r.top()    + b.get_pos().y,
            r.right()  + b.get_pos().x,
            r.bottom() + b.get_pos().y);

  // check, if stuck
  if (tilemap_collision (a.tilemap, r))
  {
    result.state=CollisionData::STUCK;
    result.col_time = 0;

    return result;
  }

  float time=0.0f;

  int next_x, next_y;   // next grid position
  float tx, ty;         // next time, when grid is hit
  float ct=1.0f;        // collision_time

  // also check at time==0
  bool first_time=true;

  bool last_zero = false;

  int maxtries = 20; // prevent loops

  while (time < delta && ct >= 0.0f && maxtries>0)
  {
    float const x = (vel.x < 0) ? r.left() : r.right();
    float const y = (vel.y < 0) ? r.top() : r.bottom();

    ct = -1.0f;

    if(first_time)
    {
      next_x = get_integer(x / static_cast<float>(TILE_SIZE), vel.x) * TILE_SIZE;
      next_y = get_integer(y / static_cast<float>(TILE_SIZE), vel.y) * TILE_SIZE;
      first_time = false;
    }
    else
    {
      next_x = get_next_integer ((x / static_cast<float>(TILE_SIZE)), vel.x) * TILE_SIZE;
      next_y = get_next_integer ((y / static_cast<float>(TILE_SIZE)), vel.y) * TILE_SIZE;


      assert ( static_cast<float>(next_x) * static_cast<float>(c_sign(vel.x)) > x * static_cast<float>(c_sign(vel.x)) || vel.x == 0.0f);
      assert ( static_cast<float>(next_y) * static_cast<float>(c_sign(vel.y)) > y * static_cast<float>(c_sign(vel.y)) || vel.y == 0.0f);
    }

    if (vel.x != 0.0f)
      tx = (static_cast<float>(next_x) - x) / vel.x;
    else
      tx = 10000.0f;

    if (vel.y != 0.0f)
      ty = (static_cast<float>(next_y) - y) / vel.y;
    else
      ty = 10000.0f;

    if(tx<0)
      tx=0;
    if(ty<0)
      ty=0;

    if (tx < ty)
    {
      if (time + tx < delta)
        ct = tx;
    }
    else
    {
      if (time + ty < delta)
        ct = ty;
    }

    if (ct >= 0.0f)
    {
      // move to next position
      float dx, dy;

      dx = ct * vel.x;
      dy = ct * vel.y;

      r = geom::frect(r.left()   + dx,
                r.top()    + dy,
                r.right()  + dx,
                r.bottom() + dy);

      if(last_zero && ct==0.0f)
        time += 0.0005f;
      else
        time += ct;
      last_zero=(ct==0.0f);

      // now shift one more pixel and check for collision with tilemap
      geom::frect tmp(r);

      if (tx < ty)
      {
        tmp = geom::frect(tmp.left() + static_cast<float>(c_sign(vel.x)),
                    tmp.top(),
                    tmp.right() + static_cast<float>(c_sign(vel.x)),
                    tmp.bottom());
      }
      else
      {
        tmp = geom::frect(tmp.left(),
                    tmp.top() + static_cast<float>(c_sign(vel.y)),
                    tmp.right(),
                    tmp.bottom() + static_cast<float>(c_sign(vel.y)));
      }

      // check collision with tilemap

      if (tilemap_collision (a.tilemap, tmp))
      {
        result.state=CollisionData::COLLISION;
        result.col_time = time;

        if (tx < ty)
          result.direction = glm::vec2(static_cast<float>(c_sign(vel.x)), 0.0f);
        else
          result.direction = glm::vec2(0, static_cast<float>(c_sign(vel.y)));
        return result;
      }
    }
    maxtries--;
  }
  if(maxtries==0)
    std::cerr<<"MAXTRIES reached"<<std::endl;

  return result;
}

glm::vec2
CollisionEngine::raycast(glm::vec2 const& pos, float angle)
{
  for(Objects::iterator i = objects.begin(); i != objects.end(); ++i)
  {
    if ((*i)->get_type() == CollisionObject::TILEMAP)
    {
      return (*i)->tilemap->raycast(pos, angle);
    }
  }

  return glm::vec2(0, 0);
}

} // namespace windstille

/* EOF */
