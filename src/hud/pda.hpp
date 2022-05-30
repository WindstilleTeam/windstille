/*
**  Windstille - A Sci-Fi Action-Adventure Game
**  Copyright (C) 2005 Ingo Ruhnke <grumbel@gmail.com>
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

#include <wstdisplay/fwd.hpp>
#include <wstgui/screen.hpp>

#include "sprite2d/sprite.hpp"

class DialogEntry
{
public:
  DialogEntry(std::string const& arg_character, std::string const& arg_text);

  std::string character;
  std::string text;
};

class ObjectiveEntry
{
public:
  ObjectiveEntry(std::string const& arg_name, std::string const& arg_text);

  std::string name;
  std::string text;
  bool complete;
};

/** */
class PDA : public wstgui::Screen
{
private:
  glm::vec2  pos;
  Sprite    background;

  std::unique_ptr<wstdisplay::TextArea>  ui_area;
  std::unique_ptr<wstdisplay::TextArea>  text_area;

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
  ~PDA() override;

  void draw(wstdisplay::GraphicsContext& gc) override;
  void update(float delta, wstinput::Controller const& controller) override;
  void add_dialog(std::string const& character, std::string const& text);

  void add_objective(std::string const& name, std::string const& text);
  void objective_complete(std::string const& name);
  bool is_objective_given(std::string const& name);
  bool is_objective_complete(std::string const& name);
};

#endif

/* EOF */
