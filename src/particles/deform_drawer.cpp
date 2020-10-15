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

#include "display/display.hpp"
#include "display/opengl_state.hpp"
#include "display/shader_object.hpp"
#include "particles/particle_system.hpp"
#include "util/pathname.hpp"

class DeformDrawerRequest : public Drawable
{
public:
  FramebufferPtr    framebuffer;
  SurfacePtr        surface;
  ParticleSystem&   psys;
  ShaderProgramPtr  shader_program;

  DeformDrawerRequest(const glm::vec2& pos_, float z_pos_,  const glm::mat4& modelview_,
                      FramebufferPtr framebuffer_, SurfacePtr surface_, ParticleSystem& psys_,
                      ShaderProgramPtr shader_program_) :
    Drawable(pos_, z_pos_, modelview_),
    framebuffer(framebuffer_), surface(surface_), psys(psys_),
    shader_program(shader_program_)
  {}

  ~DeformDrawerRequest() override {}

  void render(GraphicsContext& gc, unsigned int mask) override
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

      glBegin(GL_QUADS);
      glTexCoord2f(0,0);
      glVertex2f(0,0);

      glTexCoord2f(800,0);
      glVertex2f(800,0);

      glTexCoord2f(800,600);
      glVertex2f(800, 600);

      glTexCoord2f(0, 600);
      glVertex2f(0,600);
      glEnd();

      glUseProgram(0);
    }
#endif
  }

  void draw_particles()
  {
    glPushMatrix();
    glMultMatrixf(glm::value_ptr(get_modelview()));

    OpenGLState state;

    state.bind_texture(surface->get_texture());
    state.set_blend_func(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    state.enable(GL_BLEND);
    state.activate();

    glBegin(GL_QUADS);
    for(ParticleSystem::Particles::iterator i = psys.begin(); i != psys.end(); ++i)
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
          float s = sinf(glm::pi<float>() * i->angle/180.0f);
          float c = cosf(glm::pi<float>() * i->angle/180.0f);
          x_rot = (width/2) * c - (height/2) * s;
          y_rot = (width/2) * s + (height/2) * c;
        }

        glColor4f(color.r, color.g, color.b, color.a);
        glTexCoord2f(0, 0);
        glVertex2f(i->x - x_rot, i->y - y_rot);
        glTexCoord2f(1, 0);
        glVertex2f(i->x + y_rot, i->y - x_rot);
        glTexCoord2f(1, 1);
        glVertex2f(i->x + x_rot, i->y + y_rot);
        glTexCoord2f(0, 1);
        glVertex2f(i->x - y_rot, i->y + x_rot);
      }
    }
    glEnd();

    glPopMatrix();
  }

  void prepare(TexturePtr screen_texture)
  {
    OpenGLState state;
    state.bind_texture(screen_texture);
    state.activate();

    glBegin(GL_QUADS);
    glTexCoord2f(0,600);
    glVertex2f(0,0);

    glTexCoord2f(800,600);
    glVertex2f(800,0);

    glTexCoord2f(800, 0);
    glVertex2f(800, 600);

    glTexCoord2f(0, 0);
    glVertex2f(0,600);
    glEnd();
  }

  bool needs_prepare() { return true; }
};

DeformDrawer::DeformDrawer(ReaderMapping const& /*props*/,
                           SurfaceManager& surface_manager) :
  framebuffer(Framebuffer::create_with_texture(GL_TEXTURE_2D, 800, 600)),
  surface(surface_manager.get(Pathname("images/particles/deform2.png"))),
  shader_program(ShaderProgram::create())
{
  shader_program->attach(ShaderObject::create_from_file(GL_FRAGMENT_SHADER, "data/shader/particledeform.frag"));
  shader_program->link();
}

DeformDrawer::~DeformDrawer()
{
}

void
DeformDrawer::draw(DrawingContext& dc, ParticleSystem& psys)
{
  dc.draw(std::make_unique<DeformDrawerRequest>(glm::vec2(400, 300), 1200, dc.get_modelview(),
                                                framebuffer, surface, psys, shader_program));
}

/* EOF */
