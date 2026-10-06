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

#include <filesystem>
#include <string>

#include <glm/glm.hpp>
#include <geom/size.hpp>
#include <surf/color.hpp>
#include <wstdisplay/blend.hpp>
#include <wstdisplay/fwd.hpp>
#include <wstdisplay/surface.hpp>
#include <wstsprite/anim_state.hpp>

#include "sprite2d/manager.hpp"

namespace windstille {

/** A 2D sprite: the animation of a loaded sprite plus how this
    instance is drawn. Copies are cheap. The SpriteManager must
    outlive the sprite. */
class Sprite
{
public:
  Sprite();

  /** Load a sprite from file, in-case the .sprite file isn't found
      a .png with the same name is used as a one image sprite */
  Sprite(std::filesystem::path const& filename, SpriteManager& sprite_manager);

  void update(float delta);

  /** Draw with the anchor of the current action at \a pos, at depth
      \a z_pos */
  void draw(wstdisplay::Canvas& canvas, glm::vec2 const& pos, float z_pos = 0.0f) const;

  /** Switch to the action \a name, throws if there is none. Resets
      the speed, mirroring, scale, color and blend mode. */
  void set_action(std::string const& name);
  std::string const& get_action() const;

  /** Mirror horizontally */
  void set_hflip(bool hflip);
  bool get_hflip() const { return m_hflip; }

  /** Playback speed factor */
  void set_speed(float speed);
  float get_speed() const;

  void set_alpha(float alpha);
  float get_alpha() const;

  /** A non-looping action reached its last frame */
  bool is_finished() const;

  void set_blend(wstdisplay::Blend blend) { m_blend = blend; }
  wstdisplay::Blend get_blend() const { return m_blend; }

  void set_color(surf::Color const& color) { m_color = color; }
  surf::Color get_color() const { return m_color; }

  /** Scale of the sprite, initially the one of the action */
  void set_scale(float s) { m_scale = s; }
  float get_scale() const { return m_scale; }

  wstdisplay::Surface get_current_surface() const;

  /** Position of the image relative to the draw position, unscaled */
  glm::vec2 get_offset() const;

  float get_width() const;
  float get_height() const;
  geom::fsize get_size() const;

  /** true if the Sprite is valid and usable, false if not */
  explicit operator bool() const;

private:
  wstsprite::SpriteData const& data() const;
  void reset_appearance();

private:
  SpriteManager const* m_manager;
  wstsprite::SpriteId m_id;
  wstsprite::AnimState m_state;
  bool m_hflip;
  wstdisplay::Blend m_blend;
  float m_scale;
  surf::Color m_color;
};

} // namespace windstille

#endif

/* EOF */
