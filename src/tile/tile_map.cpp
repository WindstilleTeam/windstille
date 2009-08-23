/*
**  Windstille - A Sci-Fi Action-Adventure Game
**  Copyright (C) 2000 Ingo Ruhnke <grumbel@gmx.de>
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

#include <sstream>
#include <stdexcept>
#include <physfs.h>
#include "tile_map.hpp"
#include "tile.hpp"
#include "tile_factory.hpp"
#include "app/globals.hpp"
#include "screen/view.hpp"
#include "display/vertex_array_drawing_request.hpp"
#include "collision/collision_engine.hpp"
#include <inttypes.h>

TileMap::TileMap(FileReader& props)
{
  int width = -1;
  int height = -1;
  z_pos = 0;
  total_time = 0;
  
  props.get("name", name);
  props.get("z-pos", z_pos);
  props.get("width", width);
  props.get("height", height);

  if(width <= 0 || height <= 0) 
    {
      throw std::runtime_error("Invalid width or height defined or "
                               "data defined before width and height");  
    }
 
  if(width <= 0 || height <= 0) 
    {
      throw std::runtime_error("Invalid width or height defined or "
                               "data defined before width and height");
    }

  std::string data_filename;
  if (props.get("data-file", data_filename))
    {
      PHYSFS_file* file = PHYSFS_openRead(data_filename.c_str());
      if (!file)
        {
          throw std::runtime_error("Couldn't open tiledata file: " + data_filename);
        }
      
      field = Field<Tile*>(width, height);

      for(int y = 0; y < height; ++y)
        for(int x = 0; x < width; ++x)
          {
            uint16_t result;
            if(PHYSFS_readULE16(file, &result) == 0) {
              std::ostringstream msg;
              msg << "Problem reading uint16 value: " << PHYSFS_getLastError();
              throw std::runtime_error(msg.str());
            }

            field(x, y) = TileFactory::current()->create(result);
          }

      PHYSFS_close(file);
    }
  else // read data directly from the levelfile
    {
      Field<int> tmpfield(width, height);

      props.get("data", tmpfield.get_vector());
  
      field = Field<Tile*>(width, height);
      for (int y = 0; y < field.get_height (); ++y) 
        {
          for (int x = 0; x < field.get_width (); ++x)
            {
              field(x, y) = TileFactory::current()->create(tmpfield(x, y));
            }
        }
    }

  if (field.size() == 0)
    throw std::runtime_error("No tiles defined in tilemap");  
}

TileMap::~TileMap()
{
}

void 
TileMap::update (float delta)
{
  total_time += delta;
}

void
TileMap::draw (SceneContext& sc)
{
  Rect clip_rect = Rect(View::current()->get_clip_rect());

  Rect rect(std::max(0, clip_rect.left/TILE_SIZE),
            std::max(0, clip_rect.top/TILE_SIZE),
            std::min(field.get_width(),  clip_rect.right/TILE_SIZE + 1),
            std::min(field.get_height(), clip_rect.bottom/TILE_SIZE + 1));
  
  std::vector<VertexArrayDrawingRequest*> requests;
  for (int y = rect.top;   y < rect.bottom; ++y)
    for (int x = rect.left; x < rect.right; ++x)
      {
        Tile* tile = field(x, y);

        if (!(tile == 0 || tile->packer < 0))
          {
            int packer = tile->packer; 

            if(packer >= int(requests.size()))
              requests.resize(packer+1);

            VertexArrayDrawingRequest*& request = requests[packer];
            if (!request)
              {
                request = new VertexArrayDrawingRequest(Vector2f(0, 0), z_pos,
                                                        sc.color().get_modelview());
                request->set_mode(GL_QUADS);
                request->set_blend_func(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
                request->set_texture(tile->texture);
              }
            
            request->texcoord(tile->uv.left, tile->uv.top);
            request->vertex(x * TILE_SIZE, y * TILE_SIZE);

            request->texcoord(tile->uv.right, tile->uv.top);
            request->vertex(x * TILE_SIZE + TILE_SIZE, y * TILE_SIZE);

            request->texcoord(tile->uv.right, tile->uv.bottom);
            request->vertex(x * TILE_SIZE + TILE_SIZE, y * TILE_SIZE + TILE_SIZE);
            
            request->texcoord(tile->uv.left, tile->uv.bottom);
            request->vertex(x * TILE_SIZE, y * TILE_SIZE + TILE_SIZE); 
          }
      }

  for(std::vector<VertexArrayDrawingRequest*>::iterator i = requests.begin(); i != requests.end(); ++i)
    {
      if (*i)
        sc.color().draw(*i);
    }
}

unsigned int
TileMap::get_pixel(int x, int y)
{
  if (x < 0 || y < 0 
      || x >= int(field.get_width())
      || y >= int(field.get_height()))
    {
      //std::cout << "Out of bounce: " << x << ", " << y << std::endl;
      return 0;
    }
  else
    {
      Tile* tile = field(x, y);
      
      if (tile)
        return tile->get_colmap();
      else
        return 0;     
    }
}

bool
TileMap::is_ground (float x, float y)
{
  int x_pos = int(x) / TILE_SIZE;
  int y_pos = int(y) / TILE_SIZE;

  if (x < 0 || x_pos >= field.get_width())
    {
      //std::cout << "TileMap::is_ground (): Out of range: " << x_pos << " " << y_pos << std::endl;
      return 1;
    }
  else if (y < 0 || y_pos >= field.get_height())
    {
      return 0;
    }

  if (field(x_pos, y_pos))
    return field(x_pos, y_pos)->get_colmap() != 0;
  else
    return 0;
}

static float find_max(float pos, float v)
{
  if (v == 0) 
    return 0;
  else if (v < 0)
    return fmodf(fmodf(pos, TILE_SIZE) + TILE_SIZE, TILE_SIZE)/fabsf(v);
  else // if (v > 0)
    {
      return fmodf(fmodf(-pos, TILE_SIZE) + TILE_SIZE, TILE_SIZE)/v;
    }
}

Vector2f
TileMap::raycast(const Vector2f& pos, float angle)
{
  ////////////////////////////////////////////
  // FIXME: This code is buggy, use code from LaserPointer instead
  ///////////////////////////////////////////////
  
  // Ray position in Tile units
  int x = static_cast<int>(pos.x / static_cast<float>(TILE_SIZE));
  int y = static_cast<int>(pos.y / static_cast<float>(TILE_SIZE));

  Vector2f direction(cosf(angle) * 100.0f, sinf(angle) * 100.0f);

  int step_x = (direction.x > 0) ? 1 : -1;
  int step_y = (direction.y > 0) ? 1 : -1;

  float tMaxX = find_max(pos.x, direction.x);
  float tMaxY = find_max(pos.y, direction.y);

  float tDeltaX = (direction.x == 0) ? 0 : fabsf(TILE_SIZE / direction.x);
  float tDeltaY = (direction.y == 0) ? 0 : fabsf(TILE_SIZE / direction.y);

  float t = 0;

  while(x >= 0 && x < get_width() &&
        y >= 0 && y < get_height())
    {
      Tile* tile = field(x, y);  
      if (tile && tile->colmap)
        {
          return pos + Vector2f(t * direction.x, t * direction.y);
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

  // Ray got out of the map
  return pos + Vector2f(t * direction.x, t * direction.y);
}

/* EOF */
