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

#ifndef HEADER_WINDSTILLE_SCENEGRAPH_FILL_SCREEN_PATTERN_DRAWABLE_HPP
#define HEADER_WINDSTILLE_SCENEGRAPH_FILL_SCREEN_PATTERN_DRAWABLE_HPP

#include "display/texture.hpp"
#include "scenegraph/drawable.hpp"
#include "scenegraph/vertex_array_drawable.hpp"

class FillScreenPatternDrawable : public Drawable
{
private:
  TexturePtr m_texture;
  glm::vec2 m_offset;

public:
  FillScreenPatternDrawable(TexturePtr texture, const glm::vec2& offset) :
    Drawable(glm::vec2(0, 0), -1000.0f),
    m_texture(texture),
    m_offset(offset)
  {}

  ~FillScreenPatternDrawable() override {}

  void set_offset(const glm::vec2& offset)
  {
    m_offset = offset;
  }

  void render(GraphicsContext& gc, unsigned int mask) override
  {
    float u = static_cast<float>(gc.size().width())  / static_cast<float>(m_texture->get_width());
    float v = static_cast<float>(gc.size().height()) / static_cast<float>(m_texture->get_height());

    float u_start = -m_offset.x / static_cast<float>(m_texture->get_width());
    float v_start = -m_offset.y / static_cast<float>(m_texture->get_height());

    u -= m_offset.x / static_cast<float>(m_texture->get_width());
    v -= m_offset.y / static_cast<float>(m_texture->get_height());

    VertexArrayDrawable va;

    va.set_blend_func(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    va.set_texture(m_texture);

    gc.push_matrix();
    gc.set_modelview(glm::mat4(1.0f));

    va.set_mode(GL_TRIANGLE_FAN);
    {
      va.texcoord(u_start, v_start);
      va.vertex(0, 0);

      va.texcoord(u, v_start);
      va.vertex(gc.size().width(), 0);

      va.texcoord(u, v);
      va.vertex(gc.size().width(), gc.size().height());

      va.texcoord(u_start, v);
      va.vertex(0, gc.size().height());
    }
    va.render(gc);

    gc.pop_matrix();
  }
};

#endif

/* EOF */
