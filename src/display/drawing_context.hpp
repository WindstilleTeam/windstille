/*
**  Windstille - A Sci-Fi Action-Adventure Game
**  Copyright (C) 2000,2005 Ingo Ruhnke <grumbel@gmx.de>
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

#ifndef HEADER_WINDSTILLE_DISPLAY_DRAWING_CONTEXT_HPP
#define HEADER_WINDSTILLE_DISPLAY_DRAWING_CONTEXT_HPP

#include "math/vector2f.hpp"
#include "math/matrix.hpp"
#include "math/rect.hpp"
#include <vector>
#include "color.hpp"
#include "drawing_request.hpp"

class Texture;
class Surface;
class SurfaceDrawingParameters;
class DrawingParameters;
class SceneContext;
class Sprite;
class Quad;

/** The DrawingContext collects all DrawingRequests and allows you to
    flush them all down to the graphics card in one run, this has the
    advantage that it is possible to z-sort, texture-id sort or
    otherwise optimize the drawing. In addition to that it also allows
    you do render the drawing commands to multiple buffers which might
    be usefull for post-processing effects and such. */
class DrawingContext
{
private:
  typedef std::vector<DrawingRequest*> DrawingRequests;
  DrawingRequests drawingrequests;

  std::vector<Matrix> modelview_stack;

public:
  DrawingContext();
  ~DrawingContext();

  /** Draws everything in the drawing context to the screen */
  void render(SceneContext& sc);

  /** Empties the drawing context */
  void clear();

  /** Fills the screen with a given color, this is different from
      clear() in that it doesn't remove other DrawingRequest from the
      queue */
  void fill_screen(const Color& color);

  void fill_pattern(const Texture& pattern, const Vector2f& offset);

  void draw_line(const Vector2f& pos1, const Vector2f& pos2, const Color& color, float z_pos = 0);

  void draw_rect(const Rectf& rect, const Color& color, float z_pos = 0);
  void fill_rect(const Rectf& rect, const Color& color, float z_pos = 0);

  /*{ */
  void draw(DrawingRequest* request);
  void draw(const Sprite&   sprite,  const Vector2f& pos, float z = 0);
  void draw(const std::string& text,    float x, float y, float z = 0);
  void draw(const Surface surface, const Vector2f& pos, float z = 0, float alpha = 0);
  void draw(const Surface surface, float x, float y, float z = 0, float alpha = 0);
  void draw(const Surface surface, const SurfaceDrawingParameters& params, float z_pos = 0);
  void draw(const Surface surface, const Vector2f& pos, const Quad& quad,
            const DrawingParameters& params, float z_pos = 0);
  /*} */
  
  /** Translate the drawing context */
  void translate(float x, float y, float z = 0.0f);

  /** Set the rotation of the drawing context */
  void rotate(float angle, float x = 0.0f, float y = 0.0f, float z = 1.0f);

  /** Set the scaling of the drawing context */
  void scale(float x, float y, float z = 1.0f);

  /** multiply the modelview of the context */
  void mult(const Matrix& matrix);

  void push_modelview();
  void pop_modelview();
  void reset_modelview();
  void set_modelview(const Matrix& matrix); 
  Matrix get_modelview() const { return modelview_stack.back(); }

  /** Return the area of the screen that will be visible*/
  Rectf get_clip_rect();
private:
  DrawingContext (const DrawingContext&);
  DrawingContext& operator= (const DrawingContext&);
};

#endif

/* EOF */
