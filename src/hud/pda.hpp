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

#ifndef HEADER_WINDSTILLE_HUD_PDA_HPP
#define HEADER_WINDSTILLE_HUD_PDA_HPP

#include <memory>
#include "sprite2d/sprite.hpp"
#include "screen/screen.hpp"

class TextArea;

class DialogEntry
{
public:
  DialogEntry(const std::string& arg_character, const std::string& arg_text);
  
  std::string character;
  std::string text;
};

class ObjectiveEntry
{
public:
  ObjectiveEntry(const std::string& arg_name, const std::string& arg_text);
  
  std::string name;
  std::string text;
  bool complete;
};

/** */
class PDA : public Screen
{
private:
  Vector2f  pos;
  Sprite    background;

  std::auto_ptr<TextArea>  ui_area;
  std::auto_ptr<TextArea>  text_area;

  std::vector<DialogEntry> dialogs;
  std::vector<ObjectiveEntry> objectives;   
  
  enum PDAState { PDA_NONE, PDA_OBJECTIVES, PDA_DIALOGS };
  PDAState state;
  PDAState old_state;

  void generate_objectives();
  void generate_dialogs();
  void force_regeneration();

public:
  PDA();
  virtual ~PDA();

  void draw();
  void update(float delta, const Controller& controller);
  void add_dialog(const std::string& character, const std::string& text);
  
  void add_objective(const std::string& name, const std::string& text);
  void objective_complete(const std::string& name);
  bool is_objective_given(const std::string& name);
  bool is_objective_complete(const std::string& name);
};

#endif

/* EOF */
