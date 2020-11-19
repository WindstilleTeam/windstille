//  $Id: editor.cxx,v 1.7 2003/10/11 08:11:59 grumbel Exp $
//
//  Pingus - A free Lemmings clone
//  Copyright (C) 2002 Ingo Ruhnke <grumbel@gmx.de>
//
//  This program is free software; you can redistribute it and/or
//  modify it under the terms of the GNU General Public License
//  as published by the Free Software Foundation; either version 2
//  of the License, or (at your option) any later version.
//
//  This program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
//
//  You should have received a copy of the GNU General Public License
//  along with this program; if not, write to the Free Software
//  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.

#include <ClanLib/gui.h>
#include <iostream>
#include "../scm_functor.hxx"
#include "../globals.hxx"
#include "../windstille_game.hxx"
#include "editor/editor.hxx"
#include "editor/tile_selector.hxx"
#include "editor/editor_tilemap.hxx"
#include "editor/tile_editor.hxx"
#include "tile_factory.hxx"
#include "gui_manager.hxx"
#include "editor.hxx"

void
editor_set_brush_tile(int i)
{
  Editor::current()->get_editor_tilemap()->brush_tile = i;
}

int
editor_get_brush_tile()
{
  return Editor::current()->get_editor_tilemap()->brush_tile;
}

int
screen_get_width()
{
  return CL_Display::get_width();
}

int
screen_get_height()
{
  return CL_Display::get_height();
}

void tilemap_set_active_layer(int i)
{
  Editor::current()->get_editor_tilemap()->set_active_layer(i);
}

void editor_set_tool(int i)
{
  Editor::current()->get_editor_tilemap()->set_tool(i);
}

CL_Component*
tile_selector_create(int x, int y, int w, int h)
{
  /*CL_Window* window = new CL_Window(CL_Rect(CL_Point(x, y),
                                            CL_Size(w*(TILE_SIZE/2), h*(TILE_SIZE/2) + 32)),
                                            "TileSelector", Editor::current()->get_component());*/
  return new TileSelector(w, h, GUIManager::current()->get_component());
}

SCM diamond_map_get_data()
{
  Field<int>* field = Editor::current()->get_editor_tilemap()->get_diamond_map();
  
  if (field)
    {
      SCM vec = SCM_EOL;
      for (Field<int>::iterator i = field->begin(); i != field->end(); ++i)
        vec = scm_cons(scm_from_int(*i), vec);

      return scm_reverse(vec);
    }
  else
    {
      return SCM_EOL;
    }
}

SCM map_get_data(int i)
{
  Field<EditorTile*>* field = Editor::current()->get_editor_tilemap()->get_map(i);
  if (field)
    {
      std::cout << ": " << field->get_width() << "x" << field->get_height() 
                << " " << field->size() << std::endl;

      SCM vec = SCM_EOL;
      for (Field<EditorTile*>::iterator i = field->begin(); i != field->end(); ++i)
        {
          vec = scm_cons(scm_from_int((*i)->get_id()), vec);
        }
      return scm_reverse(vec);
    }
  else

    {
      return SCM_BOOL_F;
    }
}

int map_get_width()
{
  return Editor::current()->get_editor_tilemap()->get_width();
}

int map_get_height()
{
  return Editor::current()->get_editor_tilemap()->get_height();
}

void map_set_size(int w, int h)
{
  //return Editor::current()->get_editor_tilemap()->get_height();
}

void map_resize(int w, int h)
{
}

void map_clear()
{
}

void
editor_load(const char* filename)
{
  Editor::current()->get_editor_tilemap()->load(filename);
}

void
editor_new(int w, int h)
{
  Editor::current()->get_editor_tilemap()->new_level(w, h);
}

void
game_play(const char* filename)
{
  std::cout << "WindstilleGame: Starting level " << filename << std::endl;
  WindstilleGame game (filename);
  game.display ();
}

CL_Component*
editor_add_tileeditor(int x, int y)
{
  CL_Component* manager = GUIManager::current()->get_component();
  return new TileEditor(x, y, manager);
}

void tileeditor_set_tile(CL_Component* comp, int id)
{
  TileEditor* tileeditor = dynamic_cast<TileEditor*>(comp);
  if (tileeditor)
    tileeditor->set_tile(TileFactory::current()->create(id));
}

SCM get_tile_def(Tile* tile)
{
  SCM lst = scm_cons(scm_from_utf8_symbol("tile"), SCM_EOL);

  if (tile)
    {
      lst = scm_cons(scm_list_n(scm_from_utf8_symbol("id"), scm_from_int(tile->id), (SCM_UNDEFINED)),
                     lst);

      lst = scm_cons(scm_list_n(scm_from_utf8_symbol("image"), scm_from_utf8_string(tile->filename.c_str()), (SCM_UNDEFINED)),
                     lst);

      lst = scm_cons(scm_list_n(scm_from_utf8_symbol("colmap"),
                            scm_from_int(tile->colmap[0]),
                            scm_from_int(tile->colmap[1]),
                            scm_from_int(tile->colmap[2]),
                            scm_from_int(tile->colmap[3]),
                            scm_from_int(tile->colmap[4]),
                            scm_from_int(tile->colmap[5]),
                            scm_from_int(tile->colmap[6]),
                            scm_from_int(tile->colmap[7]),
                            SCM_UNDEFINED),
                    lst);
    }
  
  return scm_reverse(lst);
}

SCM get_tile_def(int id)
{
  return get_tile_def(TileFactory::current()->create(id));
}

SCM get_tile_defs()
{
  SCM lst = scm_cons(scm_from_utf8_symbol("windstille-tiles"), SCM_EOL);
  
  for (TileFactory::iterator i = TileFactory::current()->begin();
       i != TileFactory::current()->end();
       ++i)
    {
      lst = scm_cons(get_tile_def((*i).second), lst);
    }

  return scm_reverse(lst);
}

SCM map_get_scripts()
{
  SCM lst = SCM_EOL;

  std::vector<std::string> scripts = Editor::current()->get_editor_tilemap()->get_scripts();
  for (std::vector<std::string>::iterator i = scripts.begin(); 
       i != scripts.end(); ++i)
    {
      lst = scm_cons(scm_from_utf8_string(i->c_str()), lst);
    }
  
  return scm_reverse(lst);
}

/* EOF */
