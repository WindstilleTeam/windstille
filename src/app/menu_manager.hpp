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

/** */
class MenuManager
{
private:

public:
  MenuManager();

  void display_option_menu();
  void display_debug_menu();
  void display_main_menu();
  void display_pause_menu();
  void display_scenario_menu();
  void display_particle_menu();
  void display_models_menu();
  void display_credits();
  void display_help();

  // Callbacks
  void menu_start_game();
  void menu_quit();
  void menu_exit();
  void menu_show_fps(int i);
  void menu_fullscreen(int i);
  void menu_continue();
  void menu_music_volume(int i);
  void menu_ambient_light(int i, int component);
  void menu_start_scenario(std::string scenario);
  void menu_show_model(std::string scenario);
  void menu_show_geometry_test();
  void menu_show_armature_test();
  void menu_show_navigation_test();
  void menu_show_particle_system(std::string file);
  void menu_gamma(int i);
  void menu_wiimote();
private:
  MenuManager (const MenuManager&);
  MenuManager& operator= (const MenuManager&);
};

extern MenuManager menu_manager;

#endif

/* EOF */
