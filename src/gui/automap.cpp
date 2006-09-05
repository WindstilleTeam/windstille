/*  $Id$
**   __      __ __             ___        __   __ __   __
**  /  \    /  \__| ____    __| _/_______/  |_|__|  | |  |   ____
**  \   \/\/   /  |/    \  / __ |/  ___/\   __\  |  | |  | _/ __ \
**   \        /|  |   |  \/ /_/ |\___ \  |  | |  |  |_|  |_\  ___/
**    \__/\  / |__|___|  /\____ /____  > |__| |__|____/____/\___  >
**         \/          \/      \/    \/                         \/
**  Copyright (C) 2005 Ingo Ruhnke <grumbel@gmx.de>
**
**  This program is free software; you can redistribute it and/or
**  modify it under the terms of the GNU General Public License
**  as published by the Free Software Foundation; either version 2
**  of the License, or (at your option) any later version.
**
**  This program is distributed in the hope that it will be useful,
**  but WITHOUT ANY WARRANTY; without even the implied warranty of
**  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
**  GNU General Public License for more details.
** 
**  You should have received a copy of the GNU General Public License
**  along with this program; if not, write to the Free Software
**  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA
**  02111-1307, USA.
*/

#include "display/surface_drawing_parameters.hpp"
#include "input/controller.hpp"
#include "sector.hpp"
#include "tile_map.hpp"
#include "display/display.hpp"
#include "automap.hpp"

namespace GUI {

Automap::Automap(const lisp::Lisp* lisp, Component* parent)
  : Component(parent)
{
  assert(0);
}

Automap::Automap(const Rectf& rect, Component* parent)
  : Component(rect, parent),
    pos(0.0f, 0.0f),
    zoom(6.0f)
{
  TileMap* tilemap = Sector::current()->get_tilemap();
  
#if SDL_BYTEORDER == SDL_BIG_ENDIAN
  SDL_Surface* image = SDL_CreateRGBSurface(SDL_SWSURFACE,
                                            tilemap->get_width(), tilemap->get_height(), 32,
                                            0xff000000, 0x00ff0000, 0x0000ff00, 0x000000ff);
#else
  SDL_Surface* image = SDL_CreateRGBSurface(SDL_SWSURFACE,
                                            tilemap->get_width(), tilemap->get_height(), 32,
                                            0x000000ff, 0x0000ff00, 0x00ff0000, 0xff000000);
#endif

  SDL_LockSurface(image);
  
  unsigned char* buffer = static_cast<unsigned char*>(image->pixels);

  for(int y = 0; y < image->h; ++y)
    for(int x = 0; x < image->w; ++x)
      {
        if (tilemap->get_pixel(x, y))
          {
            buffer[image->pitch * y + 4*x + 0] = 255;
            buffer[image->pitch * y + 4*x + 1] = 255;
            buffer[image->pitch * y + 4*x + 2] = 255;
            buffer[image->pitch * y + 4*x + 3] = 255;
          }
        else
          {
            buffer[image->pitch * y + 4*x + 0] = 0;
            buffer[image->pitch * y + 4*x + 1] = 0;
            buffer[image->pitch * y + 4*x + 2] = 0;
            buffer[image->pitch * y + 4*x + 3] = 255;
          }
      }

  surface = Surface(tilemap->get_width(), tilemap->get_height());
  surface.get_texture().set_filter(GL_NEAREST);
  surface.get_texture().put(image, 0, 0);

  SDL_UnlockSurface(image);

  SDL_FreeSurface(image);
}

Automap::~Automap()
{
}

void
Automap::draw()
{
  Display::push_cliprect(Rect(get_screen_rect()));
  surface.draw(SurfaceDrawingParameters()
               .set_pos(Vector(rect.left, rect.top) + pos)
               .set_color(Color(1.0f, 1.0f, 1.0f, is_active() ? 0.8f : 0.5f))
               .set_scale(zoom));
  Display::pop_cliprect();
}

void
Automap::update(float delta, const Controller& controller)
{
  if (controller.get_button_state(AIM_BUTTON))
    {
      if (controller.get_axis_state(Y_AXIS) < 0)
        zoom /= 1.0f + (0.5f * fabsf(controller.get_axis_state(Y_AXIS))) * delta;
      else if (controller.get_axis_state(Y_AXIS) > 0)
        zoom *= 1.0f + (0.5f * controller.get_axis_state(Y_AXIS)) * delta;
    }
  else
    {
      pos.x += controller.get_axis_state(X_AXIS) * delta * 100.0f;
      pos.y += controller.get_axis_state(Y_AXIS) * delta * 100.0f;
    }

  for(InputEventLst::const_iterator i = controller.get_events().begin(); i != controller.get_events().end(); ++i) 
    {
      if (i->type == BUTTON_EVENT && i->button.down)
        {
          if (i->button.name == OK_BUTTON)
            {
            }
          else if (i->button.name == CANCEL_BUTTON)
            {
              set_active(false);
            }
        }
    }
}

} // namespace GUI

/* EOF */
