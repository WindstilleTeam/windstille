/*
**  Windstille - A Sci-Fi Action-Adventure Game
**  Copyright (C) 2009 Ingo Ruhnke <grumbel@gmail.com>
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

#ifndef HEADER_WINDSTILLE_SCENEGRAPH_SHOCKWAVE_DRAWABLE_HPP
#define HEADER_WINDSTILLE_SCENEGRAPH_SHOCKWAVE_DRAWABLE_HPP

#include "scenegraph/drawable.hpp"

#include <glm/ext.hpp>

#include "scenegraph/vertex_array_drawable.hpp"

class ShockwaveDrawable : public Drawable
{
public:
  TexturePtr noise;
  ShaderProgramPtr shader_program;
  float radius;

  ShockwaveDrawable(const glm::vec2& pos_,
                    TexturePtr      noise_,
                    ShaderProgramPtr shader_program_,
                    float r,
                    const glm::mat4& modelview_) :
    Drawable(pos_, 500.0f, modelview_),
    noise(noise_),
    shader_program(shader_program_),
    radius(r)
  {
  }

  ~ShockwaveDrawable() override
  {
  }

#if 0
  void prepare(const Texture& screen_texture)
  {
    // FIXME: Clear stuff is only for debugging
    glClearColor(1.0, 0.0, 1.0, 1.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glPushMatrix();
    glMultMatrixf(modelview.matrix);
    glTranslatef(pos.x, pos.y, 0);

    { // Apply modelview matrix to texture matrix so that we can
      // give texcoords as screencords
      GLdouble m[16];
      glMatrixMode(GL_MODELVIEW);
      glGetDoublev(GL_MODELVIEW_MATRIX, m);
      glMatrixMode(GL_TEXTURE);
      glLoadIdentity();
      glTranslatef(0, 599, 0);
      glScalef(1, -1, 1);
      glMultMatrixd(m);

      //glLoadMatrixd(m);
    }

    int count = int(radius);

    VertexArrayDrawable va;
    va.set_texture(screen_texture, 0);
    state.activate();

    float rad = static_cast<float>(count)*2.0f + 20.0f; // enlarge radius by 20.0f to handle texture displacement
    float minradius = 2.0f * static_cast<float>(count) - 164.0f;
    if (minradius < 0)
      minradius = 0;

    int segments = 64;

    va.set_mode(GL_QUADS);
    for (int i = 0; i < segments; ++i)
    {
      float angel = glm::two_pi<float>() / static_cast<float>(segments);

      float x1 =  sinf(angel*static_cast<float>(i))*rad;
      float y1 = -cosf(angel*static_cast<float>(i))*rad;

      float x2 =  sinf(angel*(static_cast<float>(i)+1))*rad;
      float y2 = -cosf(angel*(static_cast<float>(i)+1))*rad;

      va.color(1.0f, 1.0f, 1.0f, 1.0f);
      va.texcoord(x1+256, (y1+256));
      va.vertex(x1+256, y1+256, 0);

      va.color(1.0f, 1.0f, 1.0f, 1.0f);
      va.texcoord(x2+256, (y2+256));
      va.vertex(x2+256, y2+256, 0);

      float x3 =  sinf(angel*static_cast<float>(i))*minradius;
      float y3 = -cosf(angel*static_cast<float>(i))*minradius;

      float x4 =  sinf(angel*(static_cast<float>(i)+1))*minradius;
      float y4 = -cosf(angel*(static_cast<float>(i)+1))*minradius;

      va.color(1.0f, 1.0f, 1.0f, 1.0f);
      va.texcoord(x4+256, (y4+256));
      va.vertex(x4+256, y4+256, 0);

      va.color(1.0f, 1.0f, 1.0f, 1.0f);
      va.texcoord(x3+256, (y3+256));
      va.vertex(x3+256, y3+256, 0);
    }
    glEnd();

    glMatrixMode(GL_TEXTURE);
    glLoadIdentity();
    glMatrixMode(GL_MODELVIEW);

    glPopMatrix();
  }
#endif

  void render(GraphicsContext& gc, unsigned int mask) override
  {
#if 0
    glPushMatrix();
    glMultMatrixf(modelview.matrix);
    glTranslatef(pos.x, pos.y, 0);
    if (0)
    {
      geom::frect rect(0, 0, 800, 600);
      // Render the screen framebuffer to the actual screen
      VertexArrayDrawable va;
      va.set_texture(tmp_texture, 0);

      va.set_mode(GL_QUADS);

      va.texcoord(rect.left, rect.bottom);
      va.vertex(rect.left/2.0f, rect.bottom/2.0f);

      va.texcoord(rect.right, rect.bottom);
      va.vertex(rect.right/2.0f, rect.bottom/2.0f);

      va.texcoord(rect.right, rect.top);
      va.vertex(rect.right/2.0f, rect.top/2.0f);

      va.texcoord(rect.left, rect.top);
      va.vertex(rect.left/2.0f, rect.top/2.0f);

      va.render(gc);
    }
    else
    {
      VertexArrayDrawable va;
      va.set_texture(tmp_texture, 0);
      va.set_texture(noise, 1);
      state.enable(GL_BLEND);
      state.set_blend_func(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
      state.activate();

      glUseProgram(shader_program.get_handle());
      shader_program.set_uniform1f("radius",   radius/512.0f*2.0f);
      shader_program.set_uniform1i("background_tex", 0);
      shader_program.set_uniform1i("noise_tex",   1);
      draw_disc(va, int(radius));
      va.render(gc);

      glUseProgram(0);
    }
    glPopMatrix();
#endif
  }

  void draw_disc(VertexArrayDrawable& va, int count)
  {
    float rad = static_cast<float>(count)*2.0f;
    float minradius = 2.0f * static_cast<float>(count) - 164.0f;
    if (minradius < 0)
      minradius = 0;

    int segments = 64;

    va.set_mode(GL_QUADS);
    for (int i = 0; i < segments; ++i)
    {
      float angel = glm::two_pi<float>() / static_cast<float>(segments);

      float x1 =  sinf(angel*static_cast<float>(i))*rad;
      float y1 = -cosf(angel*static_cast<float>(i))*rad;

      float x2 =  sinf(angel*(static_cast<float>(i)+1))*rad;
      float y2 = -cosf(angel*(static_cast<float>(i)+1))*rad;

      va.color({1.0f, 1.0f, 1.0f, 1.0f});
      va.texcoord(x1/512.0f+0.5f, y1/512.0f+0.5f);
      va.vertex(x1 + 256.0f, y1 + 256.0f, 0.0f);

      va.color({1.0f, 1.0f, 1.0f, 1.0f});
      va.texcoord(x2/512.0f+0.5f, y2/512.0f+0.5f);
      va.vertex(x2+256, y2+256, 0.0f);

      float x3 =  sinf(angel*static_cast<float>(i))*minradius;
      float y3 = -cosf(angel*static_cast<float>(i))*minradius;

      float x4 =  sinf(angel*(static_cast<float>(i)+1))*minradius;
      float y4 = -cosf(angel*(static_cast<float>(i)+1))*minradius;

      va.color({1.0f, 1.0f, 1.0f, 0.0f});
      va.texcoord(x4/512.0f+0.5f, y4/512.0f+0.5f);
      va.vertex(x4+256, y4+256, 0.0f);

      va.color({1.0f, 1.0f, 1.0f, 0.0f});
      va.texcoord(x3/512.0f+0.5f, y3/512.0f+0.5f);
      va.vertex(x3+256, y3+256, 0.0f);
    }
  }
};

#endif

/* EOF */
