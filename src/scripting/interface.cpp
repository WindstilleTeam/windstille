/*
**  Windstille - A Sci-Fi Action-Adventure Game
**  Copyright (C) 2000,2005 Ingo Ruhnke <grumbel@gmail.com>
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

#include "scripting/wrapper.interface.hpp"

#include <sstream>

#include <sexp/value.hpp>
#include <sexp/io.hpp>

#include "app/app.hpp"
#include "app/config.hpp"
#include "app/sound_manager.hpp"
#include "display/opengl_window.hpp"
#include "engine/camera.hpp"
#include "engine/sector.hpp"
#include "engine/squirrel_thread.hpp"
#include "font/fonts.hpp"
#include "hud/conversation.hpp"
#include "hud/dialog_manager.hpp"
#include "hud/pda.hpp"
#include "hud/speech_manager.hpp"
#include "screen/game_session.hpp"

namespace Scripting
{

void set_sector(const std::string& filename)
{
  GameSession::current()->change_sector(Pathname(filename));
}

void play_music(const std::string& musicfile)
{
  g_app.sound().play_music(Pathname(musicfile, Pathname::kDataPath));
}

void stop_music(bool fade)
{
  g_app.sound().stop_music(fade);
}

void play_sound(const std::string& soundfile)
{
  g_app.sound().play(Pathname(soundfile, Pathname::kDataPath));
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

namespace {

bool camera_path_continue;
std::vector<Camera::PathPoint> camera_path;

} // namespace

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
  camera_path.push_back(Camera::PathPoint(Vector2f(x, y), zoom));
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

void wait(HSQUIRRELVM vm, float time)
{
  std::shared_ptr<SquirrelThread> ptr = ScriptManager::current()->get_thread(vm);

  if (ptr.get())
  {
    ptr->set_wakeup_event(ScriptManager::TIME, time);
  }
}

void wait_for_dialog(HSQUIRRELVM vm)
{
  std::shared_ptr<SquirrelThread> ptr = ScriptManager::current()->get_thread(vm);

  if (ptr.get())
  {
    ptr->set_wakeup_event(ScriptManager::DIALOG_CLOSED);
  }
}

void wait_for_camera(HSQUIRRELVM vm)
{
  std::shared_ptr<SquirrelThread> ptr = ScriptManager::current()->get_thread(vm);

  if (ptr.get())
  {
    ptr->set_wakeup_event(ScriptManager::CAMERA_DONE);
  }
}

void wait_for_fade(HSQUIRRELVM vm)
{
  std::shared_ptr<SquirrelThread> ptr = ScriptManager::current()->get_thread(vm);

  if (ptr.get())
  {
    ptr->set_wakeup_event(ScriptManager::FADE_DONE);
  }
}

int speech_show(const std::string& text, float x, float y, float r, float g, float b)
{
  return SpeechManager::current()->add(text, Vector2f(x, y), Color(r, g, b));
}

void wait_for_speech(HSQUIRRELVM vm, int i)
{
  ScriptManager::WakeupData data;
  data.type = ScriptManager::SPEECH_DONE;
  data.id   = i;
  ScriptManager::current()->get_thread(vm)->set_wakeup_event(data);
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

void list_objects()
{
  const std::vector<std::shared_ptr< ::GameObject > >& objects = Sector::current()->get_objects();

  for(std::vector<std::shared_ptr< ::GameObject > >::const_iterator i = objects.begin(); i != objects.end(); ++i)
  {
    if (!(*i)->get_name().empty())
      ConsoleLog << (*i)->get_name() << std::endl;
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

void wait_for_conversation(HSQUIRRELVM vm)
{
  std::shared_ptr<SquirrelThread> ptr = ScriptManager::current()->get_thread(vm);

  if (ptr.get())
  {
    ptr->set_wakeup_event(ScriptManager::CONVERSATION_CLOSED);
  }
}

SQInteger display(HSQUIRRELVM v)
{
  ConsoleLog << squirrel2string(v, -1);
  sq_pop(v, 1);
  return 0;
}

SQInteger println(HSQUIRRELVM v)
{
  ConsoleLog << squirrel2string(v, -1) << std::endl;
  sq_pop(v, 1);
  return 0;
}

SQInteger print_stack(HSQUIRRELVM v) __custom("")
{
  SQInteger top = sq_gettop(v);
  for(int i = 1; i <= top; ++i)
    ConsoleLog << i << " " << squirrel2string(v, i) << std::endl;

  return 0;
}

void set_console_font(const std::string& font, int size)
{
  try
  {
    Fonts::current()->ttffont.reset(new TTFFont(Pathname("fonts/" + font), size));
  }
  catch(std::exception& err)
  {
    ConsoleLog << err.what() << std::endl;
  }
}

void set_gamma(float g)
{
  OpenGLWindow::current()->set_gamma(g, g, g);
}

void set_gamma_rgb(float r, float g, float b)
{
  OpenGLWindow::current()->set_gamma(r, g, b);
}

void show_config()
{
  config.debug_print(ConsoleLog);
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

int render_mask_get()
{
  return GameSession::current()->get_scene_context()->get_render_mask();
}

void render_mask_set(int mask)
{
  GameSession::current()->get_scene_context()->set_render_mask(mask);
}

SQInteger spawn_object(HSQUIRRELVM v)
{
  if (Sector::current())
  {
    const char* objname = nullptr;
    sq_getstring(v, -2, &objname);

    // Newly created objects are deleted in ~SExprFileReader() and ~Lisp()
    std::vector<sexp::Value> entries;
    entries.push_back(sexp::Value::symbol(objname));
    table_to_lisp(v, -1, entries);

    try
    {
      assert(false && "spawn_object is broken");
      //SExprFileReader reader(new sexp::Value::array(entries));
      //Sector::current()->add_object(reader);
    }
    catch (std::exception& e)
    {
      std::cerr << "Error parsing object in spawn_object: " << e.what()
                << "\n";
    }
  }
  return 0;
}

void spawn_script(const std::string& filename)
{
  if (ScriptManager::current())
  {
    Pathname path = Sector::current()->get_directory();
    path.append_path(filename);
    ScriptManager::current()->run_script_file(path);
  }
}

SQInteger spawn_function(HSQUIRRELVM v)
{
  if (ScriptManager::current())
  {
    std::shared_ptr<SquirrelThread> thread = ScriptManager::current()->create_script(v, false);
    thread->load(v, -1);
    sq_pop(v, 1);
  }
  return 0;
}

SQInteger lisp2string(HSQUIRRELVM v)
{
  std::vector<sexp::Value> entries;

  table_to_lisp(v, -1, entries);

  for(std::vector<sexp::Value>::iterator i = entries.begin(); i != entries.end(); ++i)
  {
    ConsoleLog << (i - entries.begin()) << ": ";
    ConsoleLog << *i;
    ConsoleLog << std::endl;
  }

  return 0;
}

} // namespace Scripting

/* EOF */
