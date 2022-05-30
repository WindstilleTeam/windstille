/*
**  Windstille - A Sci-Fi Action-Adventure Game
**  Copyright (C) 2005,2007 Matthias Braun <matze@braunis.de>,
**                          Ingo Ruhnke <grumbel@gmail.com>
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

#ifndef HEADER_WINDSTILLE_SPRITE2D_SPRITE_HPP
#define HEADER_WINDSTILLE_SPRITE2D_SPRITE_HPP

#include <GL/glew.h>

#include <surf/color.hpp>
#include <wstdisplay/drawing_context.hpp>
#include <wstdisplay/fwd.hpp>
#include <wstdisplay/surface.hpp>

#include "sprite2d/manager.hpp"

struct SpriteAction;

class Sprite
{
private:
  /** Pointer to the Sprites data which is shared among all sprites
      with are loaded from the same file */
  SpriteDataPtr data;
  SpriteAction const* current_action;

  float frame;
  float speed;

  bool   pingpong;
  bool   reverse;
  bool   vflip;
  GLenum blend_sfactor;
  GLenum blend_dfactor;
  float  scale;
  surf::Color  color;

public:
  Sprite();

  /** Load a sprite from file or in-case the .sprite file isn't found
      search for a .png with the same name and use that as a simple
      one-file sprite */
  explicit Sprite(std::filesystem::path const& filename, SpriteManager& sprite_manager);
  explicit Sprite(SpriteDataPtr data);
  Sprite(Sprite const&);
  Sprite& operator=(Sprite const&);
  ~Sprite();

  void update(float delta);

  void draw(wstdisplay::GraphicsContext& gc, glm::vec2 const& pos) const;
  void draw(wstdisplay::DrawingContext& ctx, glm::vec2 const& pos, float z_pos = 0.0f);

  void  set_action(std::string const& name);
  std::string const& get_action() const;

  void set_vflip(bool vflip);
  bool get_vflip() const;

  void set_pingpong(bool pingpong);
  bool get_pingpong() const;

  void  set_speed(float speed);
  float get_speed() const;

  void  set_alpha(float alpha);
  float get_alpha() const;

  bool is_finished() const;

  void   set_blend_func(GLenum sfactor, GLenum dfactor);
  GLenum get_blend_sfactor() const;
  GLenum get_blend_dfactor() const;

  void set_color(surf::Color const& color);
  surf::Color get_color() const;

  void  set_scale(float s);
  float get_scale() const;

  wstdisplay::SurfacePtr get_current_surface() const;

  glm::vec2 get_offset() const;

  float get_width() const;
  float get_height() const;
  geom::fsize get_size() const;

  /** true if the Sprite is valid and usable, false if not */
  explicit operator bool() const;
};

#endif

/* EOF */
