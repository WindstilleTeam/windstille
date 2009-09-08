/*
**  Windstille - A Sci-Fi Action-Adventure Game
**  Copyright (C) 2009 Ingo Ruhnke <grumbel@gmx.de>
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

#ifndef HEADER_WINDSTILLE_EDITOR_DOCUMENT_HPP
#define HEADER_WINDSTILLE_EDITOR_DOCUMENT_HPP

#include <boost/scoped_ptr.hpp>

class UndoManager;
class SectorModel;

class Document // FIXME: name is not so great
{
private:
  boost::scoped_ptr<UndoManager> m_undo_manager;
  boost::scoped_ptr<SectorModel> m_sector_model;

public:
  Document();
  ~Document();

  UndoManager& get_undo_manager() const { return *m_undo_manager; }
  SectorModel& get_sector_model() const { return *m_sector_model; }

  void undo();
  void redo();

private:
  Document(const Document&);
  Document& operator=(const Document&);
};

#endif

/* EOF */
