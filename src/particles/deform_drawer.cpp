/*
**  Windstille - A Sci-Fi Action-Adventure Game
**  Copyright (C) 2005 Ingo Ruhnke <grumbel@gmx.de>
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
#include <config.h>

#include <GL/glew.h>
#include <GL/gl.h>

#include "display/display.hpp"
#include "display/shader_object.hpp"
#include "particle_system.hpp"
#include "display/drawing_request.hpp"
#include "display/opengl_state.hpp"
#include "deform_drawer.hpp"

class DeformDrawerRequest : public DrawingRequest
{
public:
  Framebuffer&      framebuffer;
  Surface&          surface;
  ParticleSystem&   psys;
  ShaderProgram&    shader_program;

  DeformDrawerRequest(const Vector& pos_, float z_pos,  const Matrix& modelview_,
                      Framebuffer& framebuffer_, Surface& surface_, ParticleSystem& psys_,
                      ShaderProgram& shader_program_)
    : DrawingRequest(pos_, z_pos, modelview_),
      framebuffer(framebuffer_), surface(surface_), psys(psys_), 
      shader_program(shader_program_)
  {}
  
  virtual ~DeformDrawerRequest() {}
  
  void draw(const Texture& tmp_texture) 
  {
    Display::push_framebuffer(framebuffer);
    glClear(GL_COLOR_BUFFER_BIT);
    draw_particles();
    Display::pop_framebuffer();

    if (1) {
      OpenGLState state;

      glUseProgramObjectARB(shader_program.get_handle());    
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

      glUseProgramObjectARB(0);
    }
  }

  void draw_particles()
  {
    glPushMatrix();
    glMultMatrixf(get_modelview().matrix);
    
    OpenGLState state;
    
    state.bind_texture(surface.get_texture());
    state.set_blend_func(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    state.enable(GL_BLEND);
    state.activate();    

    glBegin(GL_QUADS);
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
                float s = sinf(M_PI * i->angle/180.0f);
                float c = cosf(M_PI * i->angle/180.0f);
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

  void prepare(const Texture& screen_texture) 
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

DeformDrawer::DeformDrawer(FileReader& props)
  : framebuffer(GL_TEXTURE_RECTANGLE_ARB, 800, 600),
    surface("images/particles/deform2.png")
{
  shader_program.attach(ShaderObject(GL_FRAGMENT_SHADER_ARB, "data/shader/particledeform.frag"));
  shader_program.link();
}

DeformDrawer::~DeformDrawer()
{
}

void
DeformDrawer::draw(DrawingContext& dc, ParticleSystem& psys)
{
  dc.draw(new DeformDrawerRequest(Vector(400, 300), 1200, dc.get_modelview(), 
                                  framebuffer, surface, psys, shader_program));
}

/* EOF */
