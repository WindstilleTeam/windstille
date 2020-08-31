//  Flexlay - A Generic 2D Game Editor
//  Copyright (C) 2002 Ingo Ruhnke <grumbel@gmail.com>
//
//  This program is free software; you can redistribute it and/or
//  modify it under the terms of the GNU General Public License
//  as published by the Free Software Foundation; either version 2
//  of the License, or (at your option) any later version.
//
//  This program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
//
//  You should have received a copy of the GNU General Public License
//  along with this program; if not, write to the Free Software
//  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.

#ifndef HEADER_WINDSTILLE_DISPLAY_GRAPHIC_CONTEXT_STATE_HPP
#define HEADER_WINDSTILLE_DISPLAY_GRAPHIC_CONTEXT_STATE_HPP

#include <memory>

#include "math/rect.hpp"
#include "math/vector2f.hpp"
#include "math/matrix.hpp"

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

  Matrix get_matrix() const;

  void push(SceneContext& sc) const;
  void pop(SceneContext& sc) const;

  /** Return a rectangle in world coordinates that represents the area
      visible on the screen */
  Rectf get_clip_rect();

  int get_width()  const;
  int get_height() const;

  /** Set the current rotation angel */
  void  set_rotation(float angle);

  /** Return the current rotation angel */
  float get_rotation();

  /** Move the center of the visible area to pos */
  void     set_pos(const Vector2f& pos);
  Vector2f get_pos() const;

  Vector2f get_offset() const;

  /** Set zoom to z, while ensuring that the screen position \a pos
      (normaly the position of the mouse pointer) stays in the same
      position even after zoomed in/out */
  void  set_zoom(const Vector2f& pos, float z);
  void  set_zoom(float z);
  float get_zoom() const;

  void zoom_to (const Rectf& rect);

  Vector2f screen_to_world(const Vector2f& pos);

  GraphicContextState clone() const;

private:
  std::shared_ptr<GraphicContextStateImpl> impl;
};

#endif

/* EOF */
