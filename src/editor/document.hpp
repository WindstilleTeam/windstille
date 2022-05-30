/*
**  Windstille - A Sci-Fi Action-Adventure Game
**  Copyright (C) 2009 Ingo Ruhnke <grumbel@gmail.com>
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

#include <memory>

#include "editor/group_command.hpp"
#include "editor/selection.hpp"
#include "editor/command.hpp"
#include "editor/layer.hpp"
#include "editor/timeline_properties.hpp"

namespace windstille {

class UndoManager;
class SectorModel;
class NavGraphNodeObjectModel;
class NavGraphEdgeObjectModel;

/**
 *  Document wraps undo/redo functionality around SectorModel and
 *  provides data and functions for editing that are not part of the
 *  central data, such as Selections and ControlPoints.
 */
class Document
{
private:
  std::unique_ptr<UndoManager> m_undo_manager;
  std::unique_ptr<SectorModel> m_sector_model;

  std::shared_ptr<GroupCommand> m_group_command;
  int m_group_command_count;
  SelectionHandle m_selection;

  std::vector<ControlPointHandle> m_control_points;

  sigc::signal<void> m_sig_on_change;

public:
  Document();
  Document(std::string const& filename);
  ~Document();

  SectorModel& get_sector_model() const { return *m_sector_model; }

  /* Undo/Redo Handling
   * @{*/
  void undo();
  void redo();

  void undo_group_begin();
  void undo_group_end();

  bool has_undo() const;
  bool has_redo() const;

  void execute(const std::function<void ()>& undo_callback,
               const std::function<void ()>& redo_callback);
  void execute(CommandHandle cmd);
  /** @} */

  /* Layer Commands
   * @{*/
  void layer_add(Gtk::TreeModel::Path const& path);
  void layer_remove(Gtk::TreeModel::Path const& path);
  /** @} */

  /* NavGraph Commands
   * @{*/
  void navgraph_node_add(std::shared_ptr<NavGraphNodeObjectModel> node);
  void navgraph_node_remove(std::shared_ptr<NavGraphNodeObjectModel> node);

  void navgraph_edge_add(LayerHandle layer,
                         std::shared_ptr<NavGraphNodeObjectModel> lhs,
                         std::shared_ptr<NavGraphNodeObjectModel> rhs);
  void navgraph_edge_remove(std::shared_ptr<NavGraphEdgeObjectModel> edge);
  /** @} */

  /* Object Commands
   * @{*/
  void object_add(LayerHandle layer, ObjectModelHandle object);
  void object_remove(ObjectModelHandle object);
  void object_set_pos(ObjectModelHandle object, glm::vec2 const& new_pos);
  /** @} */

  /* Timeline Commands
   * @{*/
  void timeline_add_keyframe(ObjectModelHandle object, TimelineProperty property, float pos);
  void timeline_add_layer(std::string const& name);
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

  void select_all();

  void set_selection(SelectionHandle const& selection);
  SelectionHandle get_selection() const { return m_selection; }
  /** @} */

  /* Control Point Stuff
   * @{*/
  ControlPointHandle get_control_point(glm::vec2 const& pos) const;
  void clear_control_points();
  void create_control_points();
  std::vector<ControlPointHandle> const& get_control_points() const { return m_control_points; }
  /** @} */

  sigc::signal<void>& signal_on_change() { return m_sig_on_change; }

private:
  void on_selection_change();
  void on_change();

  void on_row_changed(Gtk::TreeModel::Path const& path, Gtk::TreeModel::iterator const& iter);
  void on_row_deleted(Gtk::TreeModel::Path const& path);
  void on_row_has_child_toggled(Gtk::TreeModel::Path const& path, Gtk::TreeModel::iterator const& iter);
  void on_row_inserted(Gtk::TreeModel::Path const& path, Gtk::TreeModel::iterator const& iter);
  void on_rows_reordered(Gtk::TreeModel::Path const& path, Gtk::TreeModel::iterator const& iter, int* new_order);

private:
  Document(Document const&);
  Document& operator=(Document const&);
};

} // namespace windstille

#endif

/* EOF */
