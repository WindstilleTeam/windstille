/*
**  Windstille - A Sci-Fi Action-Adventure Game
**  Copyright (C) 2000 Ingo Ruhnke <grumbel@gmail.com>
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

const int TILE_SIZE       = 32;
const int TILE_RESOLUTION = 64;

const float GRAVITY = 1500;

/**
 * This is the global game time in seconds. Should be the same as normal time,
 * except that it stops when the game pauses
 */
extern float game_time;

extern float game_speed;

extern bool collision_debug;

#endif

/* EOF */
