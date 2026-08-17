//  $Id: tile_selector.cxx,v 1.7 2003/09/23 19:10:05 grumbel Exp $
//
//  Pingus - A free Lemmings clone
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

#include <iostream>
#include <ClanLib/display.h>
#include "../globals.hxx"
#include "../tile_factory.hxx"
#include "../tile.hxx"
#include "scripting/editor.hxx"
#include "tile_selector.hxx"

TileSelector::TileSelector(int width, int height, CL_Component* parent)
  : CL_Component(CL_Rect(CL_Point(0,0), CL_Size(width * (TILE_SIZE/2), height * (TILE_SIZE/2))), parent),
    width(width), height(height)
{
  index = 0;

  // ClanLib leaves clipping off by default; without it scrolled tiles
  // paint outside the widget (and the parent window client area).
  set_clipping(true);

  slots.connect(sig_paint(),      this, &TileSelector::draw);
  slots.connect(sig_mouse_move(), this, &TileSelector::mouse_move);
  slots.connect(sig_mouse_down(), this, &TileSelector::mouse_down);
  slots.connect(sig_mouse_up  (), this, &TileSelector::mouse_up);
 
  mouse_over_tile = -1;
  scrolling = false;
  offset = 0;
}

void
TileSelector::mouse_up(const CL_InputEvent& event)
{
  if (event.id == CL_MOUSE_MIDDLE)
    {
      scrolling = false;
      release_mouse();
    }
}

void
TileSelector::mouse_down(const CL_InputEvent& event)
{
  if (event.id == CL_MOUSE_LEFT)
    {
      editor_set_brush_tile(mouse_over_tile);
    }
  else if (event.id == CL_MOUSE_MIDDLE)
    {
      scrolling = true;
      mouse_pos = event.mouse_pos;
      old_offset = offset;
      capture_mouse();
    }
  else if (event.id == CL_MOUSE_WHEEL_UP)
    {
      offset -= TILE_SIZE/2;
      if (offset < 0)
        offset = 0;
    }
  else if (event.id == CL_MOUSE_WHEEL_DOWN)
    {
      offset += TILE_SIZE/2;
    }
}

void
TileSelector::mouse_move(const CL_InputEvent& event)
{
  int x = event.mouse_pos.x/(TILE_SIZE/2);
  int y = (event.mouse_pos.y+offset)/(TILE_SIZE/2);

  mouse_over_tile = y * width + x;

  if (scrolling)
    {
      offset = old_offset + (mouse_pos.y - event.mouse_pos.y);
      if (offset < 0)
        offset = 0;
    }
}

void 
TileSelector::draw()
{
  // ClanLib 1.0 begin_paint only sets clipping, not a translation —
  // draw in screen coordinates relative to this widget.
  const int ox = get_screen_x();
  const int oy = get_screen_y();
  const int tile_size = TILE_SIZE / 2;

  // Only iterate tiles that can intersect the clipped viewport.
  const int start_y = offset / tile_size;
  const int end_y   = start_y + height + 1;

  CL_Display::push_modelview();
  CL_Display::add_translate(ox, oy - offset);
  for(int y = start_y; y < end_y; ++y)
    for(int x = 0; x < width; ++x)
      {
        int i = width * y + x;
        Tile* tile = TileFactory::current()->create(i);
        if (tile)
          {
            CL_Sprite sprite = tile->sur;
            sprite.set_scale(0.5f, 0.5f);
            sprite.draw(x * tile_size, y * tile_size);
            CL_Display::draw_rect(CL_Rect(CL_Point(x * tile_size, y * tile_size),
                                          CL_Size(tile_size, tile_size)),
                                  CL_Color(0,0,0,128));
          }

        if (i == editor_get_brush_tile())
          {
            CL_Display::fill_rect(CL_Rect(CL_Point(x * tile_size, y * tile_size),
                                          CL_Size(tile_size, tile_size)),
                                  CL_Color(0,0,255, 100));
          }
        else if (mouse_over_tile == i && has_mouse_over())
          {
            CL_Display::fill_rect(CL_Rect(CL_Point(x * tile_size, y * tile_size),
                                          CL_Size(tile_size, tile_size)),
                                  CL_Color(0,0,255, 20));
          }
      }
  CL_Display::pop_modelview();
}

/* EOF */
