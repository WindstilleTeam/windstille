/*
**  Windstille - A Sci-Fi Action-Adventure Game
**  Copyright (C) 2000,2005 Ingo Ruhnke <grumbel@gmail.com>
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

#include "util/file_reader.cpp"
#include "scenegraph/vertex_array_drawable.hpp"
#include "particles/particle_system.hpp"
#include "display/surface_manager.hpp"
#include "display/drawing_context.hpp"
#include "particles/surface_drawer.hpp"

SurfaceDrawer::SurfaceDrawer(SurfacePtr surface_) :
  surface(surface_),
  blendfunc_src(),
  blendfunc_dest(),
  buffer()
{
}

static GLenum string2blendfunc(const std::string& str)
{
  if (str == "src_alpha")
  {
    return GL_SRC_ALPHA;
  }
  else if (str == "one_minus_src_alpha")
  {
    return GL_ONE_MINUS_SRC_ALPHA;
  }
  else if (str == "dst_alpha")
  {
    return GL_DST_ALPHA;
  }
  else if (str == "one_minus_dst_alpha")
  {
    return GL_ONE_MINUS_SRC_ALPHA;
  }
  else if (str == "one")
  {
    return GL_ONE;
  }
  else if (str == "zero")
  {
    return GL_ZERO;
  }
  // FIXME: Implement the rest blendfunc here
  else
  {
    std::cout << "string2blendfunc: Unknown blendfunc: '" << str << "'" << std::endl;
    return GL_ONE;
  }
}

SurfaceDrawer::SurfaceDrawer(ReaderMapping const& props)
  : surface(),
    blendfunc_src(),
    blendfunc_dest(),
    buffer()
{
  std::string blendfunc_src_str = "src_alpha";
  std::string blendfunc_dst_str = "one_minus_src_alpha";
  std::string surface_file;

  props.read("image", surface_file);
  props.read("blendfunc-src", blendfunc_src_str);
  props.read("blendfunc-dst", blendfunc_dst_str);

  surface = SurfaceManager::current()->get(Pathname(surface_file));

  blendfunc_src  = string2blendfunc(blendfunc_src_str);
  blendfunc_dest = string2blendfunc(blendfunc_dst_str);

  // FIXME: Bad idea, as the psys isn't fully loaded as this point
  buffer.reset(new VertexArrayDrawable(Vector2f(), 0.0f,
                                       Matrix(1.0f)));
}

SurfaceDrawer::~SurfaceDrawer()
{
}

void
SurfaceDrawer::set_texture(SurfacePtr surface_)
{
  surface = surface_;
}

void
SurfaceDrawer::set_blendfuncs(GLenum blendfunc_src_, GLenum blendfunc_dest_)
{
  blendfunc_src  = blendfunc_src_;
  blendfunc_dest = blendfunc_dest_;
}

void
SurfaceDrawer::draw(const ParticleSystem& psys) const
{
  buffer->clear();
  buffer->set_pos(Vector2f(psys.get_x_pos(), psys.get_y_pos()));

  buffer->set_mode(GL_QUADS);
  buffer->set_texture(surface->get_texture());
  buffer->set_blend_func(blendfunc_src, blendfunc_dest);

  for(ParticleSystem::const_iterator i = psys.begin(); i != psys.end(); ++i)
  {
    if (i->t != -1.0f)
    {
      float p = 1.0f - psys.get_progress(i->t);
      Color color(psys.get_color_start().r * p + psys.get_color_stop().r * (1.0f - p),
                  psys.get_color_start().g * p + psys.get_color_stop().g * (1.0f - p),
                  psys.get_color_start().b * p + psys.get_color_stop().b * (1.0f - p),
                  psys.get_color_start().a * p + psys.get_color_stop().a * (1.0f - p));

      // scale
      float scale  = psys.get_size_start() +
        psys.get_progress(i->t) * (psys.get_size_stop() - psys.get_size_start());

      float width  = surface->get_width()  * scale;
      float height = surface->get_height() * scale;

      // rotate
      float x_rot = width/2;
      float y_rot = height/2;

      if (i->angle != 0)
      {
        float s = sinf(math::pi * i->angle/180.0f);
        float c = cosf(math::pi * i->angle/180.0f);
        x_rot = (width/2) * c - (height/2) * s;
        y_rot = (width/2) * s + (height/2) * c;
      }

      buffer->add_texcoords(surface->get_uv());

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

  buffer->render(~0u);
}

/* EOF */
