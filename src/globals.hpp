//  $Id$
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

#ifndef GLOBALS_HXX
#define GLOBALS_HXX

#include <string>

typedef enum { WEST, EAST} Direction;

extern int TILE_SIZE;
extern int TILE_RESOLUTION;

const float GRAVITY = 1500;

/** datadir => /usr/local/share/games/windstille/ */
extern std::string datadir;

extern float game_speed;

extern bool debug;
extern bool collision_debug;
extern bool sprite3dview;
extern bool sprite2dview;
extern bool particleview;

enum GameMainState { RUN_GAME, LOAD_MENU, LOAD_GAME_SESSION, QUIT_GAME };

#endif

/* EOF */
