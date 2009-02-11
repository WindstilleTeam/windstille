/*
**  Windstille - A Sci-Fi Action-Adventure Game
**  Copyright (C) 2000,2005 Ingo Ruhnke <grumbel@gmx.de>
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

#ifndef HEADER_WINDSTILLE_DIALOG_MANAGER_HPP
#define HEADER_WINDSTILLE_DIALOG_MANAGER_HPP

#include <vector>
#include <string>
#include "sprite2d/sprite.hpp"
#include "globals.hpp"
#include "screen.hpp"

class TextArea;

/** */
class DialogManager : public Screen
{
private:
  void create_text();
  
  Sprite portrait;
  std::string text;
  float delay;
  TextArea* text_area;

  enum Alignment {
    VCENTER = 0x00,
    LEFT    = 0x01,
    RIGHT   = 0x02,
    HCENTER = 0x00,
    TOP     = 0x10,
    BOTTOM  = 0x20
  };
  int alignment;
  bool caption;
  
  static const int dialog_width = 600;
  static const int portrait_border_x = 10;
  static const int portrait_border_y = 10;
  static const int text_border_x = 10;
  static const int text_border_y = 10;
  static const int portrait_width = 180;
  static const int portrait_height = 192;
  static const int outer_border_x = 20;
  int outer_border_y;

  static DialogManager* current_;
public:
  static DialogManager* current() { return current_; }

  DialogManager();
  ~DialogManager();

  void draw();
  void update(float delta, const Controller& controller);

  void add_dialog(int alignment, const std::string& portrait, const std::string& text);
  void add_caption(int alignment, const std::string& text);
};

#endif

/* EOF */
