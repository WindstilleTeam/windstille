//  $Id: windstille_editor.hxx,v 1.1 2003/08/10 19:56:40 grumbel Exp $
// 
//  Pingus - A free Lemmings clone
//  Copyright (C) 2000 Ingo Ruhnke <grumbel@gmx.de>
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

#ifndef WINDSTILLEEDITOR_HXX
#define WINDSTILLEEDITOR_HXX

class EditorTileMap;
class EditorTileMapView;

class WindstilleEditor
{
private:
  EditorTileMap* tilemap;
  EditorTileMapView* view;

public:
  WindstilleEditor ();

  /** Launch the editor and give controll to it */
  void display ();
  void load (const std::string&);
};

#endif

/* EOF */