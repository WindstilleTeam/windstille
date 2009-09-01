/*
**  Windstille - A Sci-Fi Action-Adventure Game
**  Copyright (C) 2009 Ingo Ruhnke <grumbel@gmx.de>
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

#include "display/opengl_state.hpp"
#include "scenegraph/drawable.hpp"

class FillScreenPatternDrawable : public Drawable
{
private:
  Texture  m_texture;
  Vector2f m_offset;

public:
  FillScreenPatternDrawable(const Texture& texture, const Vector2f& offset)
    : Drawable(Vector2f(0, 0), -1000.0f), 
      m_texture(texture),
      m_offset(offset)
  {}

  virtual ~FillScreenPatternDrawable() {}

  void set_offset(const Vector2f& offset)
  {
    m_offset = offset;
  }

  void draw() 
  {
    OpenGLState state;
    state.enable(GL_BLEND);
    state.set_blend_func(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    state.bind_texture(m_texture);
    state.activate();

    float u = static_cast<float>(Display::get_width())  / static_cast<float>(m_texture.get_width());
    float v = static_cast<float>(Display::get_height()) / static_cast<float>(m_texture.get_height());

    float u_start = -m_offset.x / static_cast<float>(m_texture.get_width());
    float v_start = -m_offset.y / static_cast<float>(m_texture.get_height());

    u -= m_offset.x / static_cast<float>(m_texture.get_width());
    v -= m_offset.y / static_cast<float>(m_texture.get_height());

    glPushMatrix();
    glLoadIdentity();

    glBegin(GL_QUADS);
    {
      glTexCoord2f(u_start, v_start);
      glVertex2f(0, 0);
    
      glTexCoord2f(u, v_start);
      glVertex2f(static_cast<float>(Display::get_width()), 0);

      glTexCoord2f(u, v);
      glVertex2f(static_cast<float>(Display::get_width()), static_cast<float>(Display::get_height()));

      glTexCoord2f(u_start, v);
      glVertex2f(0,  static_cast<float>(Display::get_height()));
    }
    glEnd();

    glPopMatrix();
  }
};

#endif

/* EOF */
