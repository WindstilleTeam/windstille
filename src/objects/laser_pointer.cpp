/*
**  Windstille - A Sci-Fi Action-Adventure Game
**  Copyright (C) 2005 Ingo Ruhnke <grumbel@gmail.com>
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

#include "scenegraph/vertex_array_drawable.hpp"
#include "objects/player.hpp"
#include "engine/sector.hpp"
#include "tile/tile_map.hpp"
#include "objects/laser_pointer.hpp"

LaserPointer::LaserPointer() :
  noise(),
  laserpointer(),
  laserpointer_light(),
  progress(),
  angle()
{
  noise = Texture::create(Pathname("images/noise2.png"));
  laserpointer = Sprite(Pathname("images/laserpointer.sprite"));
  laserpointer_light = Sprite(Pathname("images/laserpointer_light.sprite"));
  laserpointer_light.set_blend_func(GL_SRC_ALPHA, GL_ONE);
  noise->set_wrap(GL_REPEAT);
  noise->set_filter(GL_LINEAR);

  progress = 0.0f;
  angle = 0.0f;
}

LaserPointer::~LaserPointer()
{
}

static float find_max(float pos, float v)
{
  if (v == 0)
  {
    return 0;
  }
  else if (v < 0)
  {
    return fmodf(fmodf(pos, static_cast<float>(TILE_SIZE)) + static_cast<float>(TILE_SIZE), static_cast<float>(TILE_SIZE))/fabsf(v);
  }
  else // if (v > 0)
  {
    return fmodf(fmodf(-pos, static_cast<float>(TILE_SIZE)) + static_cast<float>(TILE_SIZE), static_cast<float>(TILE_SIZE))/v;
  }
}

void
LaserPointer::draw(SceneContext& sc)
{
  TileMap* tilemap = Sector::current()->get_tilemap();
  if (tilemap)
  {
    Vector2f pos = Player::current()->get_pos();
    pos.y -= 80;
    Vector2f target; // = Sector::current()->get_collision_engine()->raycast(pos, angle);

    // Ray position in Tile units
    int x = static_cast<int>(pos.x / static_cast<float>(TILE_SIZE));
    int y = static_cast<int>(pos.y / static_cast<float>(TILE_SIZE));

    Vector2f direction(cosf(angle) * 100.0f, sinf(angle) * 100.0f);

    int step_x = (direction.x > 0) ? 1 : -1;
    int step_y = (direction.y > 0) ? 1 : -1;

    float tMaxX = find_max(pos.x, direction.x);
    float tMaxY = find_max(pos.y, direction.y);

    float tDeltaX = (direction.x == 0) ? 0 : fabsf(static_cast<float>(TILE_SIZE) / direction.x);
    float tDeltaY = (direction.y == 0) ? 0 : fabsf(static_cast<float>(TILE_SIZE) / direction.y);

    float t = 0;

    while(x >= 0 && x < tilemap->get_width() &&
          y >= 0 && y < tilemap->get_height())
    {
      //sc.color().fill_rect(Rectf(Vector2f(x * TILE_SIZE, y * TILE_SIZE), Size(TILE_SIZE, TILE_SIZE)),
      //                     Color(1.0, 1.0, 1.0, 0.5), 500);

      if (tilemap->get_pixel(x, y))
      {
        //return Vector2f(x * TILE_SIZE, y * TILE_SIZE);
        goto done;
      }

      // move one tile
      if (tMaxX < tMaxY)
      {
        t = tMaxX;
        tMaxX += tDeltaX;
        x = x + step_x;
      }
      else
      {
        t = tMaxY;
        tMaxY += tDeltaY;
        y = y + step_y;
      }
    }

  done:
    target = pos + Vector2f(t * direction.x, t * direction.y);

    Vector2f ray = target - pos;

    VertexArrayDrawable* array = new VertexArrayDrawable(Vector2f(0,0), 10000,
                                                         sc.highlight().get_modelview());
    array->set_mode(GL_LINES);
    array->set_texture(noise);
    array->set_blend_func(GL_SRC_ALPHA, GL_ONE);

    array->color(Color(1.0f, 0.0f, 0.0f, 1.0f));
    array->texcoord(0, progress);
    array->vertex(0, 0);

    array->color(Color(1.0f, 0.0f, 0.0f, 1.0f));
    array->texcoord(glm::length(target - pos)/256.0f, progress);
    array->vertex(ray.x, ray.y);

    sc.highlight().draw(array);
    laserpointer.set_blend_func(GL_SRC_ALPHA, GL_ONE);
    sc.highlight().draw(laserpointer, ray);
    sc.light().draw(laserpointer_light, ray);
  }
}

void
LaserPointer::update(float delta)
{
  progress += 0.1f * delta;
}

float
LaserPointer::get_angle() const
{
  return angle;
}

void
LaserPointer::set_angle(float a)
{
  angle = a;
}

/* EOF */
