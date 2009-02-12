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

#include <vector>
#include "wrapper.interface.hpp"
#include "util.hpp"
#include "interface.hpp"
#include "sound/sound_manager.hpp"
#include "screen/game_session.hpp"
#include "hud/dialog_manager.hpp"
#include "hud/conversation.hpp"
#include "engine/script_manager.hpp"
#include "engine/sector.hpp"
#include "font/fonts.hpp"
#include "engine/camera.hpp"
#include "config.hpp"
#include "hud/pda.hpp"
#include "util/sexpr_file_reader.hpp"
#include "display/display.hpp"
#include "hud/controller_help_window.hpp"

namespace Scripting
{

void set_sector(const std::string& filename)
{
  GameSession::current()->change_sector(filename);
}

void play_music(const std::string& musicfile)
{
  sound_manager->play_music(musicfile);
}

void stop_music(float fadetime)
{
  sound_manager->stop_music(fadetime);
}

void play_sound(const std::string& soundfile)
{
  sound_manager->play(soundfile);
}

void caption_clear()
{
  DialogManager::current()->add_caption(0, "");
}

void caption_add(int alignment, const std::string& text)
{
  DialogManager::current()->add_caption(alignment, text);
  GameSession::current()->set_control_state(GameSession::DIALOG);
}

void caption_end()
{
  GameSession::current()->set_control_state(GameSession::GAME);
}

void camera_set_pos(float x, float y)
{
  Camera::current()->set_pos(x, y);
}

void camera_set_zoom(float z)
{
  Camera::current()->set_zoom(z);
}

bool camera_path_continue;
std::vector<Camera::PathPoint> camera_path;

void camera_continue_path()
{
  camera_path_continue = true;
  camera_path.clear();
}

void camera_begin_path()
{
  camera_path_continue = false;
  camera_path.clear();
}

void camera_add_point(float x, float y, float zoom)
{
  camera_path.push_back(Camera::PathPoint(Vector(x, y), zoom));
}

void camera_end_path()
{
  Camera::current()->set_path(camera_path, camera_path_continue);
}

void camera_set_active(bool active)
{
  // FIXME: This function is only for backward compability
  if (active)
    Camera::current()->set_mode(Camera::CAMERA_FOLLOW_PLAYER);
  else
    Camera::current()->set_mode(Camera::CAMERA_INACTIVE);
}

void set_controller_help_active(bool active)
{
  ControllerHelpWindow::current()->set_active(active);
}

void wait(HSQUIRRELVM vm, float time)
{
  script_manager->set_wakeup_event(vm, ScriptManager::TIME, time);
}

void wait_for_dialog(HSQUIRRELVM vm)
{
  script_manager->set_wakeup_event(vm, ScriptManager::DIALOG_CLOSED);
}

void wait_for_camera(HSQUIRRELVM vm)
{
  script_manager->set_wakeup_event(vm, ScriptManager::CAMERA_DONE);
}

void wait_for_fade(HSQUIRRELVM vm)
{
  script_manager->set_wakeup_event(vm, ScriptManager::FADE_DONE);
}

void dialog_show(int alignment, const std::string& character, const std::string& portrait, const std::string& text)
{
  DialogManager::current()->add_dialog(alignment, portrait, text);
  GameSession::current()->set_control_state(GameSession::DIALOG);
  GameSession::current()->get_pda().add_dialog(character, text);
}

void add_objective(const std::string& name, const std::string& text)
{
  GameSession::current()->get_pda().add_objective(name, text);
}

void objective_complete(const std::string& name)
{
  GameSession::current()->get_pda().objective_complete(name);
}

bool is_objective_given(const std::string& name)
{
  return GameSession::current()->get_pda().is_objective_given(name);
}

bool is_objective_complete(const std::string& name)
{
  return GameSession::current()->get_pda().is_objective_given(name);
}

bool run_before(HSQUIRRELVM vm)
{
  return script_manager->run_before(vm);
}

void load_state(HSQUIRRELVM v, const std::string& filename)
{
  sq_pushroottable(v);
  load_squirrel_table(v, -1, filename);
  sq_pop(v, 1);
}

void save_state(HSQUIRRELVM v, const std::string& filename)
{
  sq_pushroottable(v);
  save_squirrel_table(v, -1, filename);
  sq_pop(v, 1);
}

void list_objects()
{
  // std::vector<::GameObject*> didn't work me, the typedef is the workaround
  typedef ::GameObject GameObject;
  std::vector<GameObject*>* objects = Sector::current()->get_objects();
  
  for(std::vector<GameObject *>::iterator i = objects->begin(); i != objects->end(); ++i)
    {
      if (!(*i)->get_name().empty())
        console << (*i)->get_name() << std::endl;
    }
}

float get_game_speed()
{
  return game_speed;
}

void set_game_speed(float v)
{
  game_speed = v;
}

void conversation_add(const std::string& text)
{
  Conversation::current()->add(text);
}

void conversation_add2(const std::string& topic, const std::string& text)
{
  Conversation::current()->add(topic, text);
}

void conversation_show()
{
  Conversation::current()->show();
  GameSession::current()->set_control_state(GameSession::CONVERSATION);
}

int  conversation_get_selection()
{
  return Conversation::current()->get_selection();
}

void wait_for_conversation(HSQUIRRELVM v)
{
  script_manager->set_wakeup_event(v, ScriptManager::CONVERSATION_CLOSED);
}

SQInteger display(HSQUIRRELVM v) __custom
{
  console << squirrel2string(v, -1);
  return 0;
}

SQInteger println(HSQUIRRELVM v) __custom
{
  console << squirrel2string(v, -1) << std::endl;
  return 0;
}

void set_debug(bool t)
{
  debug = t;
}

bool get_debug()
{
  return debug;
}

void set_console_font(const std::string& font, int size)
{
  TTFFont* oldfont = Fonts::ttffont;

  try {
    Fonts::ttffont = new TTFFont("fonts/" + font, size);
    delete oldfont;
  } catch(std::exception& err) {
    console << err.what() << std::endl;
  }
}

void set_gamma(float g)
{
  Display::set_gamma(g, g, g);
}

void set_gamma_rgb(float r, float g, float b)
{
  Display::set_gamma(r, g, b);
}

void show_config()
{
  config.debug_print(console);
}

void cutscene_begin()
{
  GameSession::current()->set_cutscene_mode(true);
}

void cutscene_end()
{
  GameSession::current()->set_cutscene_mode(false);
}

void internal_fadeout_rgb(float time, float r, float g, float b)
{
  GameSession::current()->fadeout(time, Color(r, g, b));
}

void internal_fadein(float time)
{
  GameSession::current()->fadein(time);
}

int  render_mask_get()
{
  return GameSession::current()->get_scene_context()->get_render_mask();
}

void render_mask_set(int mask)
{
  GameSession::current()->get_scene_context()->set_render_mask(mask);
}

SQInteger spawn_object(HSQUIRRELVM v)
{
  using namespace lisp;
  
  const char* objname;
  sq_getstring(v, 2, &objname);

  std::vector<lisp::Lisp*> entries;
  entries.push_back(new Lisp(Lisp::TYPE_SYMBOL, objname));
  table_to_lisp(v, 3, entries);

  try {
    SExprFileReader reader(new Lisp(entries), true);
    Sector::current()->add_object(reader);
  } catch(std::exception& e) {
    std::cerr << "Error parsing object in spawn_object: " << e.what()
      << "\n";
  }

  return 0;
}

} // namespace Scripting

/* EOF */
