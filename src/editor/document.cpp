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

#include "editor/document.hpp"

#include <boost/bind.hpp>

#include "editor/selection.hpp"
#include "editor/undo_manager.hpp"
#include "editor/sector_model.hpp"
#include "editor/editor_window.hpp"
#include "editor/object_model.hpp"
#include "editor/decal_object_model.hpp"

#include "editor/group_command.hpp"
#include "editor/object_commands.hpp"
#include "editor/functor_command.hpp"
#include "editor/layer_commands.hpp"

Document::Document()
  : m_undo_manager(new UndoManager()),
    m_sector_model(new SectorModel()),
    m_selection(Selection::create()),
    m_control_points(),
    m_sig_on_change()
{
}

Document::~Document()
{
}

void
Document::undo()
{
  m_undo_manager->undo();
}

void
Document::redo()
{
  m_undo_manager->redo();
}

bool
Document::has_undo() const
{
  return m_undo_manager->can_undo();
}

bool
Document::has_redo() const
{
  return m_undo_manager->can_redo();
}

void
Document::execute(CommandHandle cmd)
{
  // FIXME: All stuff should be routed through this
  m_undo_manager->execute(cmd);
  EditorWindow::current()->update_undo_state();
  m_sector_model->rebuild_scene_graph();
  m_sig_on_change();
}

void
Document::layer_add(const Gtk::TreeModel::Path& path)
{
  execute(CommandHandle(new LayerAddCommand(*m_sector_model, path)));
}

void
Document::layer_remove(const Gtk::TreeModel::Path& path)
{
  execute(CommandHandle(new LayerDeleteCommand(*m_sector_model, path)));
}

void
Document::object_add(LayerHandle layer, ObjectModelHandle object)
{
  execute(CommandHandle(new ObjectAddCommand(layer, object)));
}

void
Document::object_remove(ObjectModelHandle object)
{
  execute(CommandHandle(new ObjectRemoveCommand(*m_sector_model, object)));
}

void
Document::object_set_pos(ObjectModelHandle object, const Vector2f& new_pos)
{
  execute(CommandHandle(new ObjectSetPosCommand(object, new_pos)));
}

void
Document::selection_raise()
{
  for(Selection::iterator i = m_selection->begin(); i != m_selection->end(); ++i)
    {
      m_sector_model->raise(*i);
    }

  m_sig_on_change();
}

void
Document::selection_lower()
{
  for(Selection::reverse_iterator i = m_selection->rbegin(); i != m_selection->rend(); ++i)
    {
      m_sector_model->lower(*i);
    }

  m_sig_on_change();
}

void
Document::selection_raise_to_top()
{
  for(Selection::reverse_iterator i = m_selection->rbegin(); i != m_selection->rend(); ++i)
    {
      m_sector_model->raise_to_top(*i);
    }

  m_sig_on_change();
}

void
Document::selection_lower_to_bottom()
{
  for(Selection::iterator i = m_selection->begin(); i != m_selection->end(); ++i)
    {
      m_sector_model->lower_to_bottom(*i);
    }

  m_sig_on_change();
}

void
Document::selection_vflip()
{
  if (!m_selection->empty())
    {
      boost::shared_ptr<GroupCommand> group_command(new GroupCommand());

      if (m_selection->size() > 1)
        {
          const Vector2f& center = m_selection->get_bounding_box().get_center();
          for(Selection::iterator i = m_selection->begin(); i != m_selection->end(); ++i)
            {
              Vector2f pos = (*i)->get_world_pos();
          
              pos.y = center.y + (center.y - pos.y);
          
              //(*i)->set_world_pos(pos);
              group_command->add(CommandHandle(new FunctorCommand(boost::bind(&ObjectModel::set_world_pos, *i, (*i)->get_world_pos()),
                                                                  boost::bind(&ObjectModel::set_world_pos, *i, pos))));
            }
        }

      for(Selection::iterator i = m_selection->begin(); i != m_selection->end(); ++i)
        {
          //(*i)->set_vflip(!(*i)->get_vflip());
          group_command->add(CommandHandle(new FunctorCommand(boost::bind(&ObjectModel::set_vflip, *i,  (*i)->get_vflip()),
                                                              boost::bind(&ObjectModel::set_vflip, *i, !(*i)->get_vflip()))));
        }

      execute(group_command);
    }
}

void
Document::selection_hflip()
{
  if (!m_selection->empty())
    {
      boost::shared_ptr<GroupCommand> group_command(new GroupCommand());

      if (m_selection->size() > 1)
        {
          const Vector2f& center = m_selection->get_bounding_box().get_center();
          for(Selection::iterator i = m_selection->begin(); i != m_selection->end(); ++i)
            {
              Vector2f pos = (*i)->get_world_pos();
          
              pos.x = center.x + (center.x - pos.x);
          
              //(*i)->set_world_pos(pos);
              group_command->add(CommandHandle(new FunctorCommand(boost::bind(&ObjectModel::set_world_pos, *i, (*i)->get_world_pos()),
                                                                  boost::bind(&ObjectModel::set_world_pos, *i, pos))));
            }
        }

      for(Selection::iterator i = m_selection->begin(); i != m_selection->end(); ++i)
        {
          //(*i)->set_hflip(!(*i)->get_hflip());
          group_command->add(CommandHandle(new FunctorCommand(boost::bind(&ObjectModel::set_hflip, *i,  (*i)->get_hflip()),
                                                              boost::bind(&ObjectModel::set_hflip, *i, !(*i)->get_hflip()))));
        }

      execute(group_command);
    }
}

void
Document::selection_connect_parent()
{
  if (m_selection->size() >= 2)
    {
      boost::shared_ptr<GroupCommand> group_command(new GroupCommand());

      ObjectModelHandle parent = *m_selection->begin();

      Selection::iterator i = m_selection->begin();
      ++i;
      for(; i != m_selection->end(); ++i)
        {
          //(*i)->set_parent(parent);
          group_command->add(CommandHandle(new FunctorCommand(boost::bind(&ObjectModel::set_parent, *i, (*i)->get_parent(), true),
                                                              boost::bind(&ObjectModel::set_parent, *i, parent, true))));
        }

      execute(group_command);
    }
}

void
Document::selection_clear_parent()
{
  boost::shared_ptr<GroupCommand> group_command(new GroupCommand());

  for(Selection::iterator i = m_selection->begin(); i != m_selection->end(); ++i)
    {
      //(*i)->set_parent(ObjectModelHandle());
      group_command->add(CommandHandle(new FunctorCommand(boost::bind(&ObjectModel::set_parent, *i, (*i)->get_parent(), true),
                                                          boost::bind(&ObjectModel::set_parent, *i, ObjectModelHandle(), true))));
    }

  execute(group_command);
}

void
Document::selection_duplicate()
{
  boost::shared_ptr<GroupCommand> group_command(new GroupCommand());
  std::map<ObjectModelHandle, ObjectModelHandle> parent_map;

  SelectionHandle new_selection = Selection::create();
  for(Selection::reverse_iterator i = m_selection->rbegin(); i != m_selection->rend(); ++i)
    {
      LayerHandle layer = m_sector_model->get_layer(*i);
      ObjectModelHandle obj = (*i)->clone();

      parent_map[*i] = obj;

      if (!layer)
        {
          EditorWindow::current()->print("Couldn't find parent layer while duplicating");
        }
      else
        {
          // Move clone a litte to make it more obvious that something happened
          obj->set_rel_pos(obj->get_rel_pos() + Vector2f(32.0f, 32.0f));
          new_selection->add(obj);

          //layer->add(obj);
          group_command->add(CommandHandle(new ObjectAddCommand(layer, obj)));
        }
    }

  // Second pass to set the parents to the cloned objects
  for(Selection::iterator i = new_selection->begin(); i != new_selection->end(); ++i)
    {
      if ((*i)->get_parent())
        {
          std::map<ObjectModelHandle, ObjectModelHandle>::iterator it = parent_map.find((*i)->get_parent());
          
          if (it == parent_map.end())
            {
              // When the parent wasn't part of the selection, leave
              // the parent untouched
            }
          else
            {
              (*i)->set_parent(it->second);
            }
        }
    }

  execute(group_command);
  set_selection(new_selection);
}

void
Document::selection_reset_rotation()
{
  boost::shared_ptr<GroupCommand> group_command(new GroupCommand());

  for(Selection::iterator i = m_selection->begin(); i != m_selection->end(); ++i)
    {
      DecalObjectModel* decal = dynamic_cast<DecalObjectModel*>(i->get());

      if (decal)
        {
          //decal->set_angle(0.0f);
          group_command->add(CommandHandle(new FunctorCommand(boost::bind(&DecalObjectModel::set_angle, decal, decal->get_angle()),
                                                              boost::bind(&DecalObjectModel::set_angle, decal, 0.0f))));
        }
    }

  on_selection_change();
  execute(group_command);
}

void
Document::selection_object_properties()
{
  std::cout << "Display Dialog" << std::endl;
}

void
Document::selection_reset_scale()
{
  boost::shared_ptr<GroupCommand> group_command(new GroupCommand());

 for(Selection::iterator i = m_selection->begin(); i != m_selection->end(); ++i)
    {
      DecalObjectModel* decal = dynamic_cast<DecalObjectModel*>(i->get());
      if (decal)
        {
          //decal->set_scale(Vector2f(1.0f, 1.0f));
          group_command->add(CommandHandle(new FunctorCommand(boost::bind(&DecalObjectModel::set_scale, decal, decal->get_scale()),
                                                              boost::bind(&DecalObjectModel::set_scale, decal, Vector2f(1.0f, 1.0f)))));
        }
    }

 on_selection_change();
 execute(group_command);
}

void
Document::selection_delete()
{
  boost::shared_ptr<GroupCommand> group_cmd(new GroupCommand());
  for(Selection::iterator i = m_selection->begin(); i != m_selection->end(); ++i)
    {
      group_cmd->add(CommandHandle(new ObjectRemoveCommand(*m_sector_model, *i)));
    }
  execute(group_cmd);
  m_selection->clear();
}

void
Document::set_selection(const SelectionHandle& selection)
{
  m_selection = selection;
  m_selection->signal_changed.connect(sigc::mem_fun(*this, &Document::on_selection_change));
  on_selection_change();
}

void
Document::on_selection_change()
{
  m_control_points.clear();

  if (!m_selection->is_moving())
    {
      m_selection->add_control_points(m_control_points);
    }
}

ControlPointHandle
Document::get_control_point(const Vector2f& pos) const
{
  for(std::vector<ControlPointHandle>::const_iterator i = m_control_points.begin();  
      i != m_control_points.end(); ++i)
    {
      if ((*i)->get_bounding_box().is_inside(pos))
        return *i;
    }
  return ControlPointHandle();
}

void
Document::clear_control_points()
{
  m_control_points.clear();
}

void
Document::create_control_points()
{
  m_control_points.clear();
  m_selection->add_control_points(m_control_points);
}


/* EOF */
