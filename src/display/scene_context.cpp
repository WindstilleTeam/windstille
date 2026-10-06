/*
**  Windstille - A Sci-Fi Action-Adventure Game
**  Copyright (C) 2026 Ingo Ruhnke <grumbel@gmail.com>
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

#include "display/scene_context.hpp"

#include <wstdisplay/renderer.hpp>
#include <wstdisplay/view.hpp>

namespace windstille {

namespace {

std::vector<wstdisplay::PassDesc> scene_passes()
{
  return {
    wstdisplay::PassDesc{.name = "color"},
    // cleared to black, the sector draws the ambient light into it
    wstdisplay::PassDesc{
      .name = "light",
      .offscreen = true,
      .resolution = 0.25f,
      .clear_color = {0.0f, 0.0f, 0.0f, 1.0f},
      .composite_blend = wstdisplay::Blend::Multiply
    },
    wstdisplay::PassDesc{.name = "highlight"},
    wstdisplay::PassDesc{.name = "control"},
  };
}

} // namespace

SceneContext::SceneContext() :
  m_compositor(scene_passes()),
  m_color(m_compositor.get_pass_index("color")),
  m_light(m_compositor.get_pass_index("light")),
  m_highlight(m_compositor.get_pass_index("highlight")),
  m_control(m_compositor.get_pass_index("control")),
  m_render_mask(COLORMAP | LIGHTMAP | HIGHLIGHTMAP | CONTROLMAP)
{
}

void
SceneContext::translate(float x, float y)
{
  for (size_t i = 0; i < m_compositor.get_pass_count(); ++i) {
    m_compositor.get_canvas(i).translate(x, y);
  }
}

void
SceneContext::rotate(float degrees)
{
  for (size_t i = 0; i < m_compositor.get_pass_count(); ++i) {
    m_compositor.get_canvas(i).rotate(degrees);
  }
}

void
SceneContext::scale(float x, float y)
{
  for (size_t i = 0; i < m_compositor.get_pass_count(); ++i) {
    m_compositor.get_canvas(i).scale(x, y);
  }
}

void
SceneContext::mult_transform(glm::mat3 const& transform)
{
  for (size_t i = 0; i < m_compositor.get_pass_count(); ++i) {
    m_compositor.get_canvas(i).mult_transform(transform);
  }
}

void
SceneContext::save()
{
  for (size_t i = 0; i < m_compositor.get_pass_count(); ++i) {
    m_compositor.get_canvas(i).save();
  }
}

void
SceneContext::restore()
{
  for (size_t i = 0; i < m_compositor.get_pass_count(); ++i) {
    m_compositor.get_canvas(i).restore();
  }
}

void
SceneContext::set_render_mask(unsigned int mask)
{
  m_render_mask = mask;
  m_compositor.get_pass(m_color).enabled = (mask & COLORMAP);
  m_compositor.get_pass(m_light).enabled = (mask & LIGHTMAP);
  m_compositor.get_pass(m_highlight).enabled = (mask & HIGHLIGHTMAP);
  m_compositor.get_pass(m_control).enabled = (mask & CONTROLMAP);
}

void
SceneContext::render(wstdisplay::Renderer& renderer, wstdisplay::View const& view)
{
  m_compositor.render(renderer, view);
}

void
SceneContext::clear()
{
  m_compositor.clear();
}

} // namespace windstille

/* EOF */
