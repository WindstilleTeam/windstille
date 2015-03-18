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

#ifndef HEADER_WINDSTILLE_SCENEGRAPH_TEXT_DRAWABLE_HPP
#define HEADER_WINDSTILLE_SCENEGRAPH_TEXT_DRAWABLE_HPP

class TextDrawable : public Drawable
{
private:
  std::string text;
public:
  TextDrawable(const std::string& text_, const Vector2f& pos_, float z_pos_, const Matrix& modelview_)
    : Drawable(pos_, z_pos_, modelview_),
      text(text_)
  {}
  virtual ~TextDrawable() {}

  void render(unsigned int mask) {
    glPushMatrix();
    glMultMatrixf(glm::value_ptr(modelview));
    Fonts::current()->ttffont->draw(pos, text);
    glPopMatrix();
  }
};

#endif

/* EOF */
