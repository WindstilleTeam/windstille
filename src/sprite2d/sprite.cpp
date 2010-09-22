/*
**  Windstille - A Sci-Fi Action-Adventure Game
**  Copyright (C) 2005,2007 Matthias Braun <matze@braunis.de>,
**                          Ingo Ruhnke <grumbel@gmx.de>
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

#include "sprite2d/data.hpp"
#include "display/surface_drawing_parameters.hpp"

Sprite::Sprite() :
  data(),
  current_action(0),
  frame(0),
  speed(0),
  pingpong(),
  reverse(),
  vflip(),
  blend_sfactor(),
  blend_dfactor(),
  scale(),
  color()
{
}

Sprite::Sprite(const Pathname& filename) :
  data(),
  current_action(0),
  frame(0),
  speed(0),
  pingpong(),
  reverse(),
  vflip(),
  blend_sfactor(),
  blend_dfactor(),
  scale(),
  color()
{
  data = SpriteManager::current()->create_data(filename);

  current_action = data->actions[0];
  vflip    = false;
  frame    = 0;
  speed    = 1.0;
  pingpong = false;
  reverse  = false;
  scale = current_action->scale;
  color    = Color(1.0f, 1.0f, 1.0f);
  blend_sfactor = GL_SRC_ALPHA;
  blend_dfactor = GL_ONE_MINUS_SRC_ALPHA;
}

Sprite::Sprite(const SpriteDataPtr data_) :
  data(data_),
  current_action(0),
  frame(0),
  speed(0),
  pingpong(),
  reverse(),
  vflip(),
  blend_sfactor(),
  blend_dfactor(),
  scale(),
  color()
{
  current_action = data->actions[0];
  vflip = false;
  frame = 0;
  speed = 1.0;
  pingpong = false;
  reverse = false;
  scale = current_action->scale;
  color    = Color(1.0f, 1.0f, 1.0f);
  blend_sfactor = GL_SRC_ALPHA;
  blend_dfactor = GL_ONE_MINUS_SRC_ALPHA;
}

Sprite::Sprite(const Sprite& rhs) :
  data(rhs.data),
  current_action(rhs.current_action),
  frame(rhs.frame),
  speed(rhs.speed),
  pingpong(rhs.pingpong),
  reverse(rhs.reverse),
  vflip(rhs.vflip),
  blend_sfactor(rhs.blend_sfactor),
  blend_dfactor(rhs.blend_dfactor),
  scale(rhs.scale),
  color(rhs.color)
{
}

Sprite&
Sprite::operator=(const Sprite& rhs)
{
  if (this != &rhs)
  {
    data           = rhs.data;
    current_action = rhs.current_action;
    frame          = rhs.frame;
    speed          = rhs.speed;
    pingpong       = rhs.pingpong;
    reverse        = rhs.reverse;
    vflip          = rhs.vflip;
    blend_sfactor  = rhs.blend_sfactor;
    blend_dfactor  = rhs.blend_dfactor;
    scale          = rhs.scale;
    color          = rhs.color; 
  }
  return *this;
}

Sprite::~Sprite()
{
}

void
Sprite::update(float delta)
{
  float step = delta * speed * current_action->speed;
  if(reverse)
    step = -step;

  frame = fmodf(frame + static_cast<float>(current_action->surfaces.size()) + step,
                static_cast<float>(current_action->surfaces.size()));
}

void
Sprite::set_action(const std::string& name)
{
  for(SpriteData::Actions::const_iterator i = data->actions.begin();
      i != data->actions.end(); ++i) 
  {
    const SpriteAction* action = *i;
    if(action->name == name) 
    {
      // FIXME: This should be per-action and not get reset, shouldn't they?
      current_action = action;
      pingpong = false;
      reverse  = false;
      speed    = 1.0;
      frame    = 0;
      vflip    = false;
      scale    = current_action->scale;
      color    = Color(1.0f, 1.0f, 1.0f);
      blend_sfactor = GL_SRC_ALPHA;
      blend_dfactor = GL_ONE_MINUS_SRC_ALPHA;
      return;
    }
  }

  std::ostringstream msg;
  msg << "No action '" << name << "' defined";
  throw std::runtime_error(msg.str());
}

void
Sprite::set_blend_func(GLenum sfactor, GLenum dfactor)
{
  blend_sfactor = sfactor;
  blend_dfactor = dfactor;
}

void
Sprite::set_vflip(bool vflip_)
{
  vflip = vflip_;
}

bool
Sprite::get_vflip() const
{
  return vflip;
}

void
Sprite::set_pingpong(bool pingpong_)
{
  pingpong = pingpong_;
}

bool
Sprite::get_pingpong() const
{
  return pingpong;
}

void
Sprite::set_speed(float speed_)
{
  speed = speed_;
}

float
Sprite::get_speed() const
{
  return speed;
}

void
Sprite::set_alpha(float alpha)
{
  color.a = alpha;
}

float
Sprite::get_alpha() const
{
  return color.a;
}

SurfacePtr
Sprite::get_current_surface() const
{
  return current_action->surfaces[ static_cast<int> (frame) ];
}

Vector2f
Sprite::get_offset() const
{
  return current_action->offset;
}

void
Sprite::draw(const Vector2f& pos) const
{
  SurfacePtr surface = current_action->surfaces[ static_cast<int> (frame) ];
  surface->draw(SurfaceDrawingParameters()
               .set_pos(pos + (current_action->offset * scale))
               .set_blend_func(blend_sfactor, blend_dfactor)
               .set_scale(scale)
               .set_color(color));
}

bool
Sprite::is_finished() const
{
  // FIXME: Implement me
  return false;
}

void
Sprite::set_scale(float s)
{
  scale = s;
}

void
Sprite::set_color(const Color& c)
{
  color = c;
}

float
Sprite::get_scale() const
{
  return scale;
}

Color
Sprite::get_color() const
{
  return color;
}

Sprite::operator bool() const
{
  return data != 0;
}

GLenum
Sprite::get_blend_sfactor() const
{
  return blend_sfactor;
}

GLenum
Sprite::get_blend_dfactor() const
{
  return blend_dfactor;
}

float
Sprite::get_width() const
{
  return current_action->surfaces[ static_cast<int>(frame) ]->get_width();
}

float
Sprite::get_height() const
{
  return current_action->surfaces[ static_cast<int>(frame) ]->get_height();
}

/* EOF */
