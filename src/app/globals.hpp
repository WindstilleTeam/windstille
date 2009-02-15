/*
**  Windstille - A Sci-Fi Action-Adventure Game
**  Copyright (C) 2000 Ingo Ruhnke <grumbel@gmx.de>
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

#ifndef HEADER_WINDSTILLE_APP_GLOBALS_HPP
#define HEADER_WINDSTILLE_APP_GLOBALS_HPP

#include <string>

typedef enum { WEST, EAST} Direction;

extern int TILE_SIZE;
extern int TILE_RESOLUTION;

const float GRAVITY = 1500;

/** datadir => /usr/local/share/games/windstille/ */
extern std::string datadir;

/**
 * This is the global game time in seconds. Should be the same as normal time,
 * except that it stops when the game pauses
 */
extern float game_time;

extern float game_speed;

extern bool debug;
extern bool collision_debug;
extern bool sprite3dview;
extern bool sprite2dview;
extern bool particleview;

enum GameMainState { RUN_GAME, LOAD_MENU, LOAD_GAME_SESSION, QUIT_GAME };

#endif

/* EOF */
