/*
**  Windstille - A Sci-Fi Action-Adventure Game
**  Copyright (C) 2000,2005 Ingo Ruhnke <grumbel@gmx.de>
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

#include "display/vertex_array_drawing_request.hpp"
#include "particle_system.hpp"
#include "../console.hpp" 
#include "display/surface_manager.hpp"
#include "surface_drawer.hpp"

SurfaceDrawer::SurfaceDrawer(Surface surface_)
  : surface(surface_)
{
  
}

static GLenum string2blendfunc(const std::string& str)
{
  if (str == "src_alpha") {
    return GL_SRC_ALPHA;
  } else if (str == "one_minus_src_alpha") {
    return GL_ONE_MINUS_SRC_ALPHA;
  } else if (str == "dst_alpha") {
    return GL_DST_ALPHA;
  } else if (str == "one_minus_dst_alpha") {
    return GL_ONE_MINUS_SRC_ALPHA;
  } else if (str == "one") {
    return GL_ONE;
  } else if (str == "zero") {
    return GL_ZERO;
  }
  // FIXME: Implement the rest blendfunc here
  else {
    console << "Unknown blendfunc: '" << str << "'" << std::endl;
    return GL_ONE;
  }
}

SurfaceDrawer::SurfaceDrawer(FileReader& props)
{
  std::string blendfunc_src_str = "src_alpha";
  std::string blendfunc_dst_str = "one_minus_src_alpha";
  std::string surface_file;

  props.get("image", surface_file);
  props.get("blendfunc-src", blendfunc_src_str);
  props.get("blendfunc-dst", blendfunc_dst_str);
  props.print_unused_warnings("SurfaceDrawer");
  
  surface = surface_manager->get(surface_file);

  blendfunc_src  = string2blendfunc(blendfunc_src_str);
  blendfunc_dest = string2blendfunc(blendfunc_dst_str);
}

SurfaceDrawer::~SurfaceDrawer() 
{
}
  
void
SurfaceDrawer::set_texture(Surface surface)
{
  this->surface = surface;
}

void
SurfaceDrawer::set_blendfuncs(GLenum blendfunc_src, GLenum blendfunc_dest)
{
  this->blendfunc_src  = blendfunc_src;
  this->blendfunc_dest = blendfunc_dest;
}

void
SurfaceDrawer::draw(DrawingContext& dc, ParticleSystem& psys) 
{          
  VertexArrayDrawingRequest* buffer 
    = new VertexArrayDrawingRequest(Vector(psys.get_x_pos(), psys.get_y_pos()), psys.get_z_pos(),
                                    dc.get_modelview());

  buffer->set_mode(GL_QUADS);
  buffer->set_texture(surface.get_texture());
  buffer->set_blend_func(blendfunc_src, blendfunc_dest);

  for(ParticleSystem::Particles::iterator i = psys.begin(); i != psys.end(); ++i)
    {
      if (i->t != -1.0f)
        {
          float p = 1.0f - psys.get_progress(i->t);
          Color color(psys.color_start.r * p + psys.color_stop.r * (1.0f - p),
                      psys.color_start.g * p + psys.color_stop.g * (1.0f - p),
                      psys.color_start.b * p + psys.color_stop.b * (1.0f - p),
                      psys.color_start.a * p + psys.color_stop.a * (1.0f - p));

          // scale
          float scale  = psys.size_start + psys.get_progress(i->t)*(psys.size_stop - psys.size_start);
          
          float width  = surface.get_width()  * scale;
          float height = surface.get_height() * scale;
              
          // rotate
          float x_rot = width/2;
          float y_rot = height/2; 

          if (i->angle != 0)
            {
              float s = sin(M_PI * i->angle/180.0f);
              float c = cos(M_PI * i->angle/180.0f);
              x_rot = (width/2) * c - (height/2) * s;
              y_rot = (width/2) * s + (height/2) * c;
            }

          buffer->add_texcoords(surface.get_uv());

          buffer->color(color);
          buffer->vertex(i->x - x_rot, i->y - y_rot);

          buffer->color(color);
          buffer->vertex(i->x + y_rot, i->y - x_rot);

          buffer->color(color);
          buffer->vertex(i->x + x_rot, i->y + y_rot);

          buffer->color(color);
          buffer->vertex(i->x - y_rot, i->y + x_rot);
        }
    }

  dc.draw(buffer);
}

/* EOF */
