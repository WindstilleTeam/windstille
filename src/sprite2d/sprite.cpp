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

#include "sprite2d/sprite.hpp"

#include <stdexcept>

#include <wstdisplay/canvas.hpp>
#include <wstdisplay/draw_params.hpp>

namespace windstille {

Sprite::Sprite() :
  m_manager(nullptr),
  m_id(),
  m_state(),
  m_hflip(false),
  m_blend(wstdisplay::Blend::Alpha),
  m_scale(1.0f),
  m_color(1.0f, 1.0f, 1.0f)
{
}

Sprite::Sprite(std::filesystem::path const& filename, SpriteManager& sprite_manager) :
  m_manager(&sprite_manager),
  m_id(sprite_manager.load(filename)),
  m_state(),
  m_hflip(false),
  m_blend(wstdisplay::Blend::Alpha),
  m_scale(1.0f),
  m_color(1.0f, 1.0f, 1.0f)
{
  reset_appearance();
}

wstsprite::SpriteData const&
Sprite::data() const
{
  return m_manager->get(m_id);
}

void
Sprite::reset_appearance()
{
  m_hflip = false;
  m_blend = wstdisplay::Blend::Alpha;
  m_scale = wstsprite::current_action(m_state, data()).scale;
  m_color = surf::Color(1.0f, 1.0f, 1.0f);
}

void
Sprite::update(float delta)
{
  wstsprite::advance(m_state, data(), delta);
}

void
Sprite::draw(wstdisplay::Canvas& canvas, glm::vec2 const& pos, float z_pos) const
{
  wstsprite::Action const& action = wstsprite::current_action(m_state, data());
  if (action.frames.empty()) {
    return;
  }

  wstdisplay::Canvas::Scope scope(canvas);
  canvas.set_z(z_pos);
  canvas.set_blend(m_blend);
  canvas.draw(action.frames[static_cast<size_t>(m_state.frame)].surface,
              wstdisplay::DrawParams()
              .set_pos(geom::fpoint(pos.x, pos.y))
              .set_offset(geom::foffset(-action.origin.x(), -action.origin.y()))
              .set_scale(m_scale)
              .set_color(m_color)
              .set_hflip(m_hflip != action.hflip));
}

void
Sprite::set_action(std::string const& name)
{
  if (!wstsprite::set_action(m_state, data(), name)) {
    throw std::runtime_error("No action '" + name + "' defined");
  }
  m_state.speed = 1.0f;
  reset_appearance();
}

std::string const&
Sprite::get_action() const
{
  return wstsprite::current_action(m_state, data()).name;
}

void
Sprite::set_hflip(bool hflip)
{
  m_hflip = hflip;
}

void
Sprite::set_speed(float speed)
{
  m_state.speed = speed;
}

float
Sprite::get_speed() const
{
  return m_state.speed;
}

void
Sprite::set_alpha(float alpha)
{
  m_color.a = alpha;
}

float
Sprite::get_alpha() const
{
  return m_color.a;
}

bool
Sprite::is_finished() const
{
  return m_state.finished;
}

wstdisplay::Surface
Sprite::get_current_surface() const
{
  return wstsprite::current_frame(m_state, data()).surface;
}

glm::vec2
Sprite::get_offset() const
{
  wstsprite::Action const& action = wstsprite::current_action(m_state, data());
  return glm::vec2(-action.origin.x(), -action.origin.y());
}

float
Sprite::get_width() const
{
  return get_current_surface().get_width();
}

float
Sprite::get_height() const
{
  return get_current_surface().get_height();
}

geom::fsize
Sprite::get_size() const
{
  return {get_width(), get_height()};
}

Sprite::operator bool() const
{
  return m_manager != nullptr && m_manager->find(m_id) != nullptr;
}

} // namespace windstille

/* EOF */
