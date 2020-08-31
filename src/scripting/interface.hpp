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

#ifndef HEADER_WINDSTILLE_SCRIPTING_INTERFACE_HPP
#define HEADER_WINDSTILLE_SCRIPTING_INTERFACE_HPP

#ifndef SCRIPTING_API
#include <string>
#include <squirrel/squirrel.h>
#endif

/*
  List of sq_setparamscheck() types:

  '|' can be used as 'or' to accept multiple type

  'o' null
  'i' integer
  'f' float
  'n' integer or float
  's' string
  't' table
  'a' array
  'u' userdata
  'c' closure and nativeclosure
  'g' generator
  'p' userpointer
  'v' thread
  'x' instance(class instance)
  'y' class
  'b' bool
  '.' any type
*/

namespace Scripting {

void set_sector(const std::string& filename);

void play_music(const std::string& musicfile);

void stop_music(bool fade);

void play_sound(const std::string& soundfile);

void caption_add(int alignment, const std::string& text);
void caption_clear();
void caption_end();

void camera_set_active(bool active);
void camera_continue_path();
void camera_begin_path();
void camera_add_point(float x, float y, float zoom);
void camera_end_path();

void camera_set_pos(float x, float y);
void camera_set_zoom(float z);

// dialog alignment constants
static const int VCENTER = 0x00;
static const int LEFT    = 0x01;
static const int RIGHT   = 0x02;
static const int HCENTER = 0x00;
static const int TOP     = 0x10;
static const int BOTTOM  = 0x20;

int  speech_show(const std::string& text, float x, float y, float r, float g, float b);
void wait_for_speech(HSQUIRRELVM vm, int i) __suspend;

void dialog_show(int alignment, const std::string& character, const std::string& portrait, const std::string& text);
void wait_for_dialog(HSQUIRRELVM vm) __suspend;
void wait_for_fade(HSQUIRRELVM vm) __suspend;
void wait_for_camera(HSQUIRRELVM vm) __suspend;

void conversation_add(const std::string& text);
void conversation_add2(const std::string& topic, const std::string& text);
void conversation_show();
int  conversation_get_selection();
void wait_for_conversation(HSQUIRRELVM v) __suspend;

void add_objective(const std::string& name, const std::string& text);
void objective_complete(const std::string& name);
bool is_objective_given(const std::string& name);
bool is_objective_complete(const std::string& name);

// for testing
void save_state(HSQUIRRELVM v, const std::string& filename);
void load_state(HSQUIRRELVM v, const std::string& filename);

void list_objects();

float get_game_speed();
void  set_game_speed(float v);

//Waits the specified time in seconds.
void wait(HSQUIRRELVM vm, float time) __suspend;

SQInteger display(HSQUIRRELVM) __custom("t.");
SQInteger println(HSQUIRRELVM) __custom("t.");
SQInteger print_stack(HSQUIRRELVM) __custom("t");

void set_console_font(const std::string& font, int size);
void set_gamma(float g);
void set_gamma_rgb(float r, float g, float b);

void show_config();

void cutscene_begin();
void cutscene_end();

void internal_fadeout_rgb(float time, float r, float g, float b);
void internal_fadein(float time);

int  render_mask_get();
void render_mask_set(int mask);

SQInteger lisp2string(HSQUIRRELVM v) __custom("tt|a");

/**
 * Spawn object. Parameters:
 *    name:    string with name of object
 *    table:   table that is parsed to get object properties
 *
 * Example: spawn_object("scriptable-object", {name="blup", pos=[350,370], sprite="images/arrow.sprite"});
 */
SQInteger spawn_object(HSQUIRRELVM v) __custom("tst");

void spawn_script(const std::string& filename);
SQInteger spawn_function(HSQUIRRELVM v) __custom("c");

} // namespace Scripting

#endif

/* EOF */
