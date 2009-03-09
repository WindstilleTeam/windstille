/*
**  Windstille - A Sci-Fi Action-Adventure Game
**  Copyright (C) 2005 Ingo Ruhnke <grumbel@gmx.de>
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

#ifndef HEADER_WINDSTILLE_APP_MENU_MANAGER_HPP
#define HEADER_WINDSTILLE_APP_MENU_MANAGER_HPP

#include <vector>
#include "math/rect.hpp"

/** */
class MenuManager 
{
public:
  static void display_option_menu();
  static void display_debug_menu();
  static void display_main_menu();
  static void display_pause_menu();
  static void display_scenario_menu();
  static void display_particle_menu();
  static void display_models_menu();
  static void display_credits();
  static void display_help();
  
private:
  static Rectf create_centered_rect(float w, float h);

  // Callbacks
  static void menu_start_game();
  static void menu_quit();
  static void menu_exit();
  static void menu_show_fps(int i);
  static void menu_fullscreen(int i);
  static void menu_continue();

  static void menu_master_volume(int i);
  static void menu_voice_volume(int i);
  static void menu_sfx_volume(int i);
  static void menu_music_volume(int i);

  static void menu_ambient_light(int i, int component);
  static void menu_start_scenario(std::string scenario);
  static void menu_show_model(std::string scenario);
  static void menu_show_geometry_test();
  static void menu_show_armature_test();
  static void menu_show_navigation_test();
  static void menu_show_particle_system(std::string file);
  static void menu_gamma(int i);
  static void menu_wiimote();
};

#endif

/* EOF */
