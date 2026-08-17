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

#ifndef HEADER_WINDSTILLE_DISPLAY_GRAPHIC_CONTEXT_STATE_HPP
#define HEADER_WINDSTILLE_DISPLAY_GRAPHIC_CONTEXT_STATE_HPP

#include <memory>

#include <glm/glm.hpp>

#include <geom/rect.hpp>

namespace wstdisplay {

class SceneContext;

class GraphicContextStateImpl;

/** Helper class for capturing the state of a GraphicContext, with
    additional convenience functions to make handling GraphicContexts
    easier */
class GraphicContextState
{
public:
  GraphicContextState();
  GraphicContextState(int w, int h);

  void set_size(int w, int h);

  glm::mat4 get_matrix() const;

  void push(SceneContext& sc) const;
  void pop(SceneContext& sc) const;

  /** Return a rectangle in world coordinates that represents the area
      visible on the screen */
  geom::frect get_clip_rect();

  int get_width()  const;
  int get_height() const;
  geom::isize get_size() const;

  /** Set the current rotation angel */
  void  set_rotation(float angle);

  /** Return the current rotation angel */
  float get_rotation();

  /** Move the center of the visible area to pos */
  void set_pos(geom::fpoint const& pos);
  geom::fpoint get_pos() const;

  geom::foffset get_offset() const;

  /** Set zoom to z, while ensuring that the screen position \a pos
      (normaly the position of the mouse pointer) stays in the same
      position even after zoomed in/out */
  void  set_zoom(geom::fpoint const& pos, float z);
  void  set_zoom(float z);
  float get_zoom() const;

  void zoom_to (const geom::frect& rect);

  geom::fpoint screen_to_world(geom::fpoint const& pos);

  GraphicContextState clone() const;

private:
  std::shared_ptr<GraphicContextStateImpl> impl;
};

} // namespace wstdisplay

#endif

/* EOF */
