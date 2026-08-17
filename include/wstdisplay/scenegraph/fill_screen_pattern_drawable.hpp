// Windstille Display Library
// Copyright (C) 2002-2020 Ingo Ruhnke <grumbel@gmail.com>
//
// This program is free software: you can redistribute it and/or modify it
// under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation, either version 3 of the License, or (at your
// option) any later version.
//
// This program is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
// or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public
// License for more details.
//
// You should have received a copy of the GNU Lesser General Public License
// along with this program. If not, see <http://www.gnu.org/licenses/>.

#ifndef HEADER_WINDSTILLE_SCENEGRAPH_FILL_SCREEN_PATTERN_DRAWABLE_HPP
#define HEADER_WINDSTILLE_SCENEGRAPH_FILL_SCREEN_PATTERN_DRAWABLE_HPP

#include <wstdisplay/texture.hpp>
#include <wstdisplay/scenegraph/drawable.hpp>
#include <wstdisplay/scenegraph/vertex_array_drawable.hpp>

namespace wstdisplay {

class FillScreenPatternDrawable : public Drawable
{
private:
  TexturePtr m_texture;
  geom::foffset m_offset;

public:
  FillScreenPatternDrawable(TexturePtr texture, const geom::foffset& offset) :
    Drawable(geom::fpoint(0.0f, 0.0f), -1000.0f),
    m_texture(texture),
    m_offset(offset)
  {}

  ~FillScreenPatternDrawable() override {}

  void set_offset(const geom::foffset& offset)
  {
    m_offset = offset;
  }

  void render(GraphicsContext& gc, unsigned int mask) override
  {
    float u = static_cast<float>(gc.size().width())  / static_cast<float>(m_texture->get_width());
    float v = static_cast<float>(gc.size().height()) / static_cast<float>(m_texture->get_height());

    float u_start = -m_offset.x() / static_cast<float>(m_texture->get_width());
    float v_start = -m_offset.y() / static_cast<float>(m_texture->get_height());

    u -= m_offset.x() / static_cast<float>(m_texture->get_width());
    v -= m_offset.y() / static_cast<float>(m_texture->get_height());

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

} // namespace wstdisplay

#endif

/* EOF */
