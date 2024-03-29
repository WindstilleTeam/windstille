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

#include "particles/deform_drawer.hpp"

#include <GL/glew.h>
#include <glm/ext.hpp>

#include <wstdisplay/graphics_context.hpp>
#include <wstdisplay/shader_object.hpp>
#include "particles/particle_system.hpp"
#include "util/pathname.hpp"
#include <wstdisplay/scenegraph/vertex_array_drawable.hpp>

namespace windstille {

class DeformDrawerRequest : public wstdisplay::Drawable
{
public:
  wstdisplay::FramebufferPtr    framebuffer;
  wstdisplay::SurfacePtr        surface;
  ParticleSystem&   psys;
  wstdisplay::ShaderProgramPtr  shader_program;

  DeformDrawerRequest(glm::vec2 const& pos_, float z_pos_,  glm::mat4 const& modelview_,
                      wstdisplay::FramebufferPtr framebuffer_,
                      wstdisplay::SurfacePtr surface_,
                      ParticleSystem& psys_,
                      wstdisplay::ShaderProgramPtr shader_program_) :
    Drawable(pos_, z_pos_, modelview_),
    framebuffer(framebuffer_), surface(surface_), psys(psys_),
    shader_program(shader_program_)
  {}

  ~DeformDrawerRequest() override {}

  void render(wstdisplay::GraphicsContext& gc, unsigned int mask) override
  {
#if 0
    Display::push_framebuffer(framebuffer);
    glClear(GL_COLOR_BUFFER_BIT);
    draw_particles();
    Display::pop_framebuffer();

    if (1) {
      OpenGLState state;

      glUseProgram(shader_program.get_handle());
      shader_program.set_uniform1i("screen",      0);
      shader_program.set_uniform1i("particles",   1);

      state.bind_texture(tmp_texture, 0);
      state.bind_texture(framebuffer.get_texture(), 1);
      state.color(Color(1.0f, 1.0f, 1.0f, 1.0f));
      state.activate();

      va.set_mode(GL_TRIANGLE_FAN);
      va.texcoord(0,0);
      va.vertex(0,0);

      va.texcoord(800,0);
      va.vertex(800,0);

      va.texcoord(800,600);
      va.vertex(800, 600);

      va.texcoord(0, 600);
      va.vertex(0,600);

      va.render(gc);

      glUseProgram(0);
    }
#endif
  }

  void draw_particles(wstdisplay::GraphicsContext& gc)
  {
    gc.push_matrix();
    gc.mult_matrix(get_modelview());

    wstdisplay::VertexArrayDrawable va;

    va.set_texture(surface->get_texture());
    va.set_blend_func(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    va.set_mode(GL_TRIANGLES);
    for(ParticleSystem::Particles::iterator i = psys.begin(); i != psys.end(); ++i)
    {
      if (i->t != -1.0f)
      {
        float p = 1.0f - psys.get_progress(i->t);
        surf::Color color(
          psys.get_color_start().r * p + psys.get_color_stop().r * (1.0f - p),
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
          float s = sinf(glm::pi<float>() * i->angle/180.0f);
          float c = cosf(glm::pi<float>() * i->angle/180.0f);
          x_rot = (width/2) * c - (height/2) * s;
          y_rot = (width/2) * s + (height/2) * c;
        }

        // v1
        va.color(color);
        va.texcoord(0, 0);
        va.vertex(i->x - x_rot, i->y - y_rot);
        // v4
        va.color(color);
        va.texcoord(0, 1);
        va.vertex(i->x - y_rot, i->y + x_rot);
        // v2
        va.color(color);
        va.texcoord(1, 0);
        va.vertex(i->x + y_rot, i->y - x_rot);

        // v4
        va.color(color);
        va.texcoord(0, 1);
        va.vertex(i->x - y_rot, i->y + x_rot);
        // v3
        va.color(color);
        va.texcoord(1, 1);
        va.vertex(i->x + x_rot, i->y + y_rot);
        // v2
        va.color(color);
        va.texcoord(1, 0);
        va.vertex(i->x + y_rot, i->y - x_rot);
      }
    }
    va.render(gc);

    gc.pop_matrix();
  }

  void prepare(wstdisplay::TexturePtr screen_texture)
  {
    wstdisplay::VertexArrayDrawable va;
    va.set_texture(screen_texture);

    va.set_mode(GL_TRIANGLE_FAN);

    va.texcoord(0,600);
    va.vertex(0,0);

    va.texcoord(800,600);
    va.vertex(800,0);

    va.texcoord(800, 0);
    va.vertex(800, 600);

    va.texcoord(0, 0);
    va.vertex(0,600);

#if 0
    va.render(gc);
#endif
  }

  bool needs_prepare() { return true; }
};

DeformDrawer::DeformDrawer(ReaderMapping const& /*props*/,
                           wstdisplay::SurfaceManager& surface_manager) :
  framebuffer(wstdisplay::Framebuffer::create_with_texture(GL_TEXTURE_2D, {800, 600})),
  surface(surface_manager.get(Pathname("images/particles/deform2.png"))),
  shader_program(wstdisplay::ShaderProgram::create())
{
  shader_program->attach(wstdisplay::ShaderObject::from_file(GL_FRAGMENT_SHADER, "data/shader/particledeform.frag"));
  shader_program->link();
}

DeformDrawer::~DeformDrawer()
{
}

void
DeformDrawer::draw(wstdisplay::DrawingContext& dc, ParticleSystem& psys)
{
  dc.draw(std::make_unique<DeformDrawerRequest>(glm::vec2(400, 300), 1200, dc.get_modelview(),
                                                framebuffer, surface, psys, shader_program));
}

} // namespace windstille

/* EOF */
