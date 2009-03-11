/*  -*- c++ -*-
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

/**
 * This script is read on Windstille startup, add all helper functions
 * that should be available in the console all the time go here. Use
 * the init.nut file for non-function stuff that should be evaluated
 * (startup messages and such).
 */

/**
 * Constants from display/scene_context.hpp, manually copied here so
 * you have to sync manually in case stuff changes
 */
RENDER_MASK_COLORMAP       <- 1<<0
RENDER_MASK_LIGHTMAP       <- 1<<1
RENDER_MASK_HIGHLIGHTMAP   <- 1<<2
RENDER_MASK_LIGHTMAPSCREEN <- 1<<3
RENDER_MASK_BLURMAP        <- 1<<4
RENDER_MASK_ALL            <- (RENDER_MASK_HIGHLIGHTMAP | 
                               RENDER_MASK_LIGHTMAP | 
                               RENDER_MASK_COLORMAP | 
                               RENDER_MASK_BLURMAP)

has_nightvision <- false;

function nightvision_enabled(...)
{
  if (vargc == 1) 
    {
      if (has_nightvision) 
        {
          objects.nightvision.set_active(vargv[0]); 
        }
      else if (vargv[0])
        {
          spawn_object("nightvision", {});
          has_nightvision = true;
        }
    } 
  else 
    {
      return true;
    }
}

function game_speed(...)
{
  if (vargc == 1) 
    {
      set_game_speed(vargv[0]);
    } else {
    return get_game_speed();
  }
}

function conversation_get()
{
  conversation_show();
  println("wait_for_conversation: " + wait_for_conversation());
  return conversation_get_selection();
}

class Dialog {
  constructor(arg_align, arg_character, arg_portrait)
  {
    align = arg_align;
    character = arg_character;
    portrait = arg_portrait;
  }

  function say(text)
  {
    ::dialog_show(align, character, portrait, text);
    ::wait_for_dialog();
  }

  function show(text)
  {
    ::dialog_show(align, character, portrait, text);
    ::wait_for_dialog();
  }

  align = null;
  character = null;
  portrait = null;
};

function add_dialog(align, character, portrait, text)
{
  dialog_show(align, character, portrait, text);
  wait_for_dialog();  
}

/* Some debugging/testing stuff */
function testo(spritename)
{
  spawn_object("test-object", {
      name = "Test",
        pos = [435, 709],
        sprite = spritename
        });
}

function fadein(...)
{
  if (vargc == 0) 
    {
      internal_fadein(1);
    } 
  else if (vargc == 1) 
    {
      internal_fadein(vargv[0]);
    } 
  else 
    {
      print("fadeout: Wrong number of arguments: " + vargc);
    }
}

function fadeout(...)
{
  if (vargc == 0)
    {
      internal_fadeout_rgb(1, 0, 0, 0);
    }
  else if (vargc == 1)
    {
      internal_fadeout_rgb(vargv[0],  0, 0, 0);
    }
  else if (vargc == 3)
    {
      internal_fadeout_rgb(1,  vargv[0], vargv[1], vargv[2]);
    }
  else if (vargc == 4)
    {
      internal_fadeout_rgb(vargv[0], vargv[1], vargv[2], vargv[3]);
    }
  else
    {
      print("fadeout: Wrong number of arguments: " + vargc);
    }
}

/* EOF */
