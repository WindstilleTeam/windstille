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

#include "editor/selection.hpp"
#include "editor/command.hpp"
#include "editor/layer.hpp"

class UndoManager;
class SectorModel;

/**
 *  Document wraps undo/redo functionality around SectorModel and
 *  provides data and functions for editing that are not part of the
 *  central data, such as Selections and ControlPoints.
 */
class Document // FIXME: name is not so great
{
private:
  boost::scoped_ptr<UndoManager> m_undo_manager;
  boost::scoped_ptr<SectorModel> m_sector_model;

  SelectionHandle m_selection;

  std::vector<ControlPointHandle> m_control_points;

  sigc::signal<void> m_sig_on_change;

public:
  Document();
  ~Document();

  SectorModel& get_sector_model() const { return *m_sector_model; }
  SelectionHandle get_selection() const { return m_selection; }

  /* Undo/Redo Handling
   * @{*/  
  void undo();
  void redo();
  bool has_undo() const;
  bool has_redo() const;
  void execute(CommandHandle cmd);
  /** @} */

  /* Layer Commands
   * @{*/
  void layer_add(const Gtk::TreeModel::Path& path);
  void layer_remove(const Gtk::TreeModel::Path& path);
  /** @} */

  /* Object Commands
   * @{*/
  void object_add(LayerHandle layer, ObjectModelHandle object);
  void object_remove(ObjectModelHandle object);
  void object_set_pos(ObjectModelHandle object, const Vector2f& new_pos);
  /** @} */

  /* Selection Commands
   * @{*/
  void selection_raise();
  void selection_lower();

  void selection_raise_to_top();
  void selection_lower_to_bottom();

  void selection_vflip();
  void selection_hflip();

  void selection_connect_parent();
  void selection_clear_parent();
  
  void selection_duplicate();
  void selection_delete();

  void selection_reset_rotation();
  void selection_reset_scale();

  void selection_object_properties();

  void set_selection(const SelectionHandle& selection);
  /** @} */

  /* Control Point Stuff
   * @{*/  
  ControlPointHandle get_control_point(const Vector2f& pos) const;
  void clear_control_points();
  void create_control_points();
  /** @} */

  sigc::signal<void>& signal_on_change() { return m_sig_on_change; }

private:
  void on_selection_change();

private:
  Document(const Document&);
  Document& operator=(const Document&);
};

#endif

/* EOF */
