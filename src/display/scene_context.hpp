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

#ifndef HEADER_WINDSTILLE_DISPLAY_SCENE_CONTEXT_HPP
#define HEADER_WINDSTILLE_DISPLAY_SCENE_CONTEXT_HPP

#include <glm/glm.hpp>

#include <wstdisplay/canvas.hpp>
#include <wstdisplay/compositor.hpp>
#include <wstdisplay/fwd.hpp>

namespace windstille {

/** The layers a game object draws to, all in world coordinates. They
    are combined into the final image by render():

    color: the regular graphics, sprites, tiles, enemies
    light: the lightmap, white is fully lit, black is dark. It is
           multiplied with the color layer, the sector fills it with
           the ambient light first, lights are drawn with Blend::Add.
    highlight: things that glow, drawn on top of the lit image so
           they stay visible in the dark
    control: editor and debug handles, drawn on top of everything

    The transform functions apply to all layers at once. */
class SceneContext final
{
public:
  enum
  {
    COLORMAP       = 1<<0,
    LIGHTMAP       = 1<<1,
    HIGHLIGHTMAP   = 1<<2,
    CONTROLMAP     = 1<<3
  };

public:
  SceneContext();

  wstdisplay::Canvas& color() { return m_compositor.get_canvas(m_color); }
  wstdisplay::Canvas& light() { return m_compositor.get_canvas(m_light); }
  wstdisplay::Canvas& highlight() { return m_compositor.get_canvas(m_highlight); }
  wstdisplay::Canvas& control() { return m_compositor.get_canvas(m_control); }

  void translate(float x, float y);
  /** Rotate by \a degrees, clockwise on screen */
  void rotate(float degrees);
  void scale(float x, float y);
  void mult_transform(glm::mat3 const& transform);

  /** Push the state of all layers, see Canvas::save() */
  void save();
  void restore();

  /** The render mask switches layers off for debugging */
  void set_render_mask(unsigned int mask);
  unsigned int get_render_mask() const { return m_render_mask; }

  /** Render the layers onto the frame's target, world coordinates
      are mapped through \a view, and clear them */
  void render(wstdisplay::Renderer& renderer, wstdisplay::View const& view);

  /** Clear the layers without rendering */
  void clear();

private:
  wstdisplay::Compositor m_compositor;
  size_t m_color;
  size_t m_light;
  size_t m_highlight;
  size_t m_control;
  unsigned int m_render_mask;

public:
  SceneContext(SceneContext const&) = delete;
  SceneContext& operator=(SceneContext const&) = delete;
};

} // namespace windstille

#endif

/* EOF */
