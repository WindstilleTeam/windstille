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

#include "math/rect.hpp"

class Pathname;

/** 
 *  The MenuManager creates and displays the metagame menus (main,
 *  pause, options, ...) in Windstille. All the input redirection and
 *  displaying is handled in ScreenManager, so MenuManager itself is
 *  completly stateless.
 */
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
  /** Position a menu with the given size \a w, \a h at the center of
      the screen */
  static Rectf create_centered_rect(float w, float h);

  /** Position a menu at the given positions, while thread the
      positions relative to the 800x600 ration */
  static Rectf create_positioned_rect(const Vector2f& pos, const Sizef& size);

  // Callbacks
  static void menu_start_game();
  static void menu_quit();
  static void menu_exit();
  static void menu_show_fps(int i);
  static void menu_fullscreen(int i);
  static void menu_controller_debug(int i);
  static void menu_continue();

  static void menu_master_volume(int i);
  static void menu_voice_volume(int i);
  static void menu_sound_volume(int i);
  static void menu_music_volume(int i);

  static void menu_ambient_light(int i, int component);
  static void menu_start_scenario(const Pathname& filename);
  static void menu_show_model(const Pathname& filename);
  static void menu_show_geometry_test();
  static void menu_show_armature_test();
  static void menu_show_navigation_test();
  static void menu_show_particle_system(const Pathname& filename);
  static void menu_gamma(int i);
  static void menu_wiimote();
};

#endif

/* EOF */
