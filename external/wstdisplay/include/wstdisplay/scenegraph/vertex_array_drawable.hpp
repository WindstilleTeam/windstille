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

#ifndef HEADER_WINDSTILLE_SCENEGRAPH_VERTEX_ARRAY_DRAWABLE_HPP
#define HEADER_WINDSTILLE_SCENEGRAPH_VERTEX_ARRAY_DRAWABLE_HPP

#include <span>
#include <unordered_map>
#include <vector>

#include <surf/color.hpp>
#include <wstdisplay/scenegraph/drawable.hpp>
#include <wstdisplay/shader_program.hpp>

namespace wstdisplay {

class VertexArrayDrawable : public Drawable
{
public:
  VertexArrayDrawable();
  VertexArrayDrawable(geom::fpoint const& pos, float z_pos, glm::mat4 const& modelview);

  void render(GraphicsContext& gc, unsigned int mask = ~0u) override;

  void normal(float x, float y, float z);

  void vertex(int x, int y, int z = 0);
  void vertex(float x, float y, float z = 0.0f);
  void vertex(geom::fpoint const& vec, float z = 0.0f);

  void texcoord(float u, float v);
  void color(surf::Color const& color);

  void add_vertices(std::span<float const> data);
  void add_texcoords(std::span<float const> data);
  void add_normals(std::span<float const> data);
  void add_indices(std::span<unsigned short int const> data);

  /** Add eight texcoords for use with a quad from a given rect. The
      coords are clockwise around the rect, ie: left, top, right, top,
      right, bottom, left, bottom */
  void add_texcoords_from_rect(geom::frect const& coords);

  int num_vertices() const;

  void clear();

  void set_program(ShaderProgramPtr program);
  void set_mode(GLenum mode_);
  void set_texture(TexturePtr texture);
  void set_texture(int unit, TexturePtr texture);
  void set_blend_func(GLenum sfactor, GLenum dfactor);
  void set_depth_test(bool depth_test);

private:
  ShaderProgramPtr m_program;
  GLenum m_mode;

  GLenum m_blend_sfactor;
  GLenum m_blend_dfactor;

  bool m_depth_test;

  std::unordered_map<int, TexturePtr> m_textures;
  std::vector<float> m_colors;
  std::vector<float> m_texcoords;
  std::vector<float> m_normals;
  std::vector<float> m_vertices;
  std::vector<unsigned short int> m_indices;
};

} // namespace wstdisplay

#endif

/* EOF */
