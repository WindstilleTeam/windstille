//  Flexlay - A Generic 2D Game Editor
//  Copyright (C) 2002 Ingo Ruhnke <grumbel@gmx.de>
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

#include "display/graphic_context_state.hpp"

#include <GL/glew.h>

#include "display/scene_context.hpp"

class GraphicContextStateImpl
{
public:
  int width;
  int height;
  
  Vector2f offset;
  float zoom;
  float rotation;

  GraphicContextStateImpl()
    : width(1),
      height(1),
      offset(0.0f, 0.0f),
      zoom(1.0f),
      rotation(0)
  {}
};

GraphicContextState::GraphicContextState()
  : impl(new GraphicContextStateImpl())
{
}

GraphicContextState::GraphicContextState(int w, int h)
  : impl(new GraphicContextStateImpl())
{  
  impl->width  = w;
  impl->height = h;
  impl->offset = Vector2f(0,0); 
  impl->zoom   = 1.0f;
  impl->rotation = 0;
}

void
GraphicContextState::set_size(int w, int h)
{
  impl->width  = w;
  impl->height = h;
}

void
GraphicContextState::push(SceneContext& sc)
{
  sc.push_modelview();

  sc.translate(static_cast<float>(impl->width)  / 2.0f, 
               static_cast<float>(impl->height) / 2.0f);
  sc.rotate(impl->rotation);
  sc.translate(static_cast<float>(-impl->width)  / 2.0f,
               static_cast<float>(-impl->height) / 2.0f);

  sc.scale(get_zoom(), get_zoom());
  sc.translate(impl->offset.x, impl->offset.y);
}

void
GraphicContextState::pop(SceneContext& sc)
{
  sc.pop_modelview();
}

Rectf
GraphicContextState::get_clip_rect()
{
  return Rectf(Vector2f(-impl->offset.x,
                         -impl->offset.y),
               Sizef(static_cast<float>(get_width())  / impl->zoom,
                     static_cast<float>(get_height()) / impl->zoom));
}
    
void
GraphicContextState::set_pos(const Vector2f& pos)
{
  impl->offset.x = -pos.x + (static_cast<float>(get_width())  / 2.0f / impl->zoom);
  impl->offset.y = -pos.y + (static_cast<float>(get_height()) / 2.0f / impl->zoom);
}

Vector2f
GraphicContextState::get_offset() const
{
  return impl->offset;
}

Vector2f
GraphicContextState::get_pos() const
{
  return Vector2f(-impl->offset.x + (static_cast<float>(get_width())  / 2.0f / impl->zoom),
                  -impl->offset.y + (static_cast<float>(get_height()) / 2.0f / impl->zoom));
}

void
GraphicContextState::set_zoom(const Vector2f& pos, float z)
{
  float old_zoom = impl->zoom;
  set_zoom(z);
  impl->offset.x = pos.x/impl->zoom - pos.x/old_zoom + impl->offset.x;
  impl->offset.y = pos.y/impl->zoom - pos.y/old_zoom + impl->offset.y;
}

void
GraphicContextState::set_zoom(float z)
{
  impl->zoom = z;
}

float
GraphicContextState::get_zoom()
{
  return impl->zoom;
}

void
GraphicContextState::zoom_to (const Rectf& rect)
{
  float center_x = (rect.left + rect.right) / 2.0f;
  float center_y = (rect.top + rect.bottom) / 2.0f;

  float width  = rect.right - rect.left;
  float height = rect.bottom - rect.top;
  float screen_relation = float(get_height())/float(get_width ());
  float rect_relation   = height/width; 
  
  //std::cout << "Screen: " << screen_relation << " Zoom: " << rect_relation << std::endl;
  if (rect_relation < screen_relation) // take width, ignore height
    {
      impl->zoom = static_cast<float>(get_width()) / width; 
    }
  else // take height, ignore width
    {
      impl->zoom = static_cast<float>(get_height()) / height;
    }

  impl->offset.x = (static_cast<float>(get_width())  / (2.0f * impl->zoom)) - center_x;
  impl->offset.y = (static_cast<float>(get_height()) / (2.0f * impl->zoom)) - center_y;
}

Vector2f
GraphicContextState::screen_to_world(const Vector2f& pos_)
{
  Vector2f pos(pos_.x, pos_.y);
  float sa = sinf(-impl->rotation / 180.0f * math::pi);
  float ca = cosf(-impl->rotation / 180.0f * math::pi);

  float dx = pos.x - static_cast<float>(impl->width)  / 2.0f;
  float dy = pos.y - static_cast<float>(impl->height) / 2.0f;

  pos.x = static_cast<float>(impl->width)  / 2.0f + (ca * dx - sa * dy);
  pos.y = static_cast<float>(impl->height) / 2.0f + (sa * dx + ca * dy);

  Vector2f p((static_cast<float>(pos.x) / impl->zoom) - impl->offset.x, 
             (static_cast<float>(pos.y) / impl->zoom) - impl->offset.y);
  
  return p;
}

void
GraphicContextState::set_rotation(float angle)
{
  impl->rotation = angle;
}

float
GraphicContextState::get_rotation()
{
  return impl->rotation;
}

int
GraphicContextState::get_width()  const 
{
  return impl->width; 
}

int
GraphicContextState::get_height() const 
{ 
  return impl->height; 
}

GraphicContextState
GraphicContextState::clone() const
{
  GraphicContextState tmp;

  tmp.impl->width    = impl->width;
  tmp.impl->height   = impl->height;
  tmp.impl->offset   = impl->offset;
  tmp.impl->zoom     = impl->zoom;
  tmp.impl->rotation = impl->rotation;

  return tmp;
}

/* EOF */
