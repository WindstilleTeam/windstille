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

#ifndef HEADER_VERTEX_ARRAY_DRAWING_REQUEST_HXX
#define HEADER_VERTEX_ARRAY_DRAWING_REQUEST_HXX

#include <vector>
#include "color.hpp"
#include "math/matrix.hpp"
#include "display/texture.hpp"
#include "drawing_request.hpp"

/** */
class VertexArrayDrawingRequest : public DrawingRequest
{
private:
  GLenum mode;

  GLenum blend_sfactor;
  GLenum blend_dfactor;

  Texture texture;
  std::vector<unsigned char> colors;
  std::vector<float> texcoords;
  std::vector<float> vertices;

public:
  VertexArrayDrawingRequest(const Vector& pos_, float z_pos_, const Matrix& modelview_);

  void draw(const Texture& tmp_texture);
  void draw(int start, int end);

  void vertex(float x, float y, float z = 0.0f);
  void vertex(const Vector& vec, float z = 0.0f);

  void texcoord(float u, float v);
  void color(const Color& color);
  void add_texcoords(const float* coords, size_t n);
  /**
   * Add eight texcoords for use with a quad from a given rect. The
   * coords are clockwise around the rect, ie:
   * left, top, right, top, right, bottom, left, bottom,
   */
  void add_texcoords(const Rectf& coords);

  int num_vertices() const;

  void set_mode(GLenum mode_);
  void set_texture(Texture texture);
  void set_blend_func(GLenum sfactor, GLenum dfactor);
};

#endif

/* EOF */
