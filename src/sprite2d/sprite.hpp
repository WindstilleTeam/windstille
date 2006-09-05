#ifndef __SPRITE2D_SPRITE_HPP__
#define __SPRITE2D_SPRITE_HPP__

#include <string>
#include <GL/glew.h>
#include <GL/gl.h>
#include "color.hpp"
#include "display/surface.hpp"
#include "math/vector.hpp"
#include "math/matrix.hpp"
#include "manager.hpp"

class DrawingContext;
class Color;

namespace sprite2d {
struct Action;
} 

class Sprite
{
public:
  Sprite();

  /** Load a sprite from file or in-case the .sprite file isn't found
      search for a .png with the same name and use that as a simple
      one-file sprite */
  Sprite(const std::string& filename);
  Sprite(const sprite2d::DataPtr data);
  ~Sprite();

  void update(float delta);
  void draw(const Vector& pos) const;

  void  set_action(const std::string& name);
  const std::string& get_action() const;

  void set_vflip(bool vflip);
  bool get_vflip() const;

  void set_pingpong(bool pingpong);
  bool get_pingpong() const;

  void  set_speed(float speed);
  float get_speed() const;

  void  set_alpha(float alpha);
  float get_alpha() const;
  
  bool is_finished() const;

  void set_blend_func(GLenum sfactor, GLenum dfactor);
  GLenum get_blend_sfactor() const;
  GLenum get_blend_dfactor() const;

  void set_color(const Color& color);
  Color get_color() const;

  void  set_scale(float s);
  float get_scale() const;

  Surface get_current_surface() const;

  Vector get_offset() const;

  int get_width() const;
  int get_height() const;

  /** true if the Sprite is valid and usable, false if not */
  operator bool() const;

private:
  /** Pointer to the Sprites data which is shared among all sprites
      with are loaded from the same file */
  sprite2d::DataPtr data;
  const sprite2d::Action* current_action;

  float frame;
  float speed;
  float alpha;

  bool pingpong;
  bool reverse;
  bool vflip;
  GLenum blend_sfactor;
  GLenum blend_dfactor;
  float scale;
  Color color;
};

#endif

