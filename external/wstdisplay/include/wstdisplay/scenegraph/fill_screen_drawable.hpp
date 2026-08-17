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

#ifndef HEADER_WINDSTILLE_SCENEGRAPH_FILL_SCREEN_DRAWABLE_HPP
#define HEADER_WINDSTILLE_SCENEGRAPH_FILL_SCREEN_DRAWABLE_HPP

namespace wstdisplay {

class FillScreenDrawable : public Drawable
{
private:
  surf::Color color;

public:
  FillScreenDrawable(const surf::Color& color_)
    : Drawable(glm::vec2(0, 0), -1000.0f), color(color_)
  {
  }
  ~FillScreenDrawable() override {}

  void render(GraphicsContext& gc, unsigned int mask) override
  {
    glClearColor(color.r, color.g, color.b, color.a);
    glClear(GL_COLOR_BUFFER_BIT);
  }
};

} // namespace wstdisplay

#endif

/* EOF */
