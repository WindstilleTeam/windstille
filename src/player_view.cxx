//  $Id: player_view.cxx,v 1.2 2003/08/12 08:24:41 grumbel Exp $
//
//  Windstille - A Jump'n Shoot Game
//  Copyright (C) 2000 Ingo Ruhnke <grumbel@gmx.de>
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

#include <ClanLib/gl.h>
#include "player.hxx"
#include "player_view.hxx"
#include "game_world.hxx"

PlayerView::PlayerView (Player* t)
  : player (t),
    world (player->get_world ())
{
  
}

void
PlayerView::draw ()
{
  //glPushMatrix ();
  //glTranslatef (-pos.x + CL_Display::get_width ()/2, -pos.y + CL_Display::get_height ()/2, 0.0);
  CL_Display::push_modelview();
  CL_Display::add_translate(int(-pos.x + CL_Display::get_width ()/2),
                            int(-pos.y + CL_Display::get_height ()/2));
  world->draw ();
  CL_Display::pop_modelview();
  //glPopMatrix ();
}

void
PlayerView::update (float delta)
{
  CL_Vector tpos = player->get_pos ();

  float dist = tpos.x - pos.x;
  if (dist > 150)
    pos.x = tpos.x - 150;
  else if (dist < -150)
    pos.x = tpos.x + 150;

  dist = tpos.y - pos.y;
  if (dist > 50)
    pos.y = tpos.y - 50;
  else if (dist < -150)
    pos.y = tpos.y + 150;

}

/* EOF */