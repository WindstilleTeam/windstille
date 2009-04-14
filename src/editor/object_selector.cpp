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

#include <iostream>
#include <boost/function.hpp>
#include <gtkmm/treeselection.h>
#include <gtkmm/stock.h>
#include <gtkmm/toolbar.h>
#include <gtkmm/treemodelcolumn.h>

#include "windstille_widget.hpp"
#include "editor_window.hpp"
#include "object_selector.hpp"

class ObjectIconColumns : public Gtk::TreeModel::ColumnRecord
{
public:
  Gtk::TreeModelColumn<std::string> url;
  Gtk::TreeModelColumn<std::string> pathname;
  Gtk::TreeModelColumn<Glib::RefPtr<Gdk::Pixbuf> >  icon;
  
  static ObjectIconColumns& instance() {
    if (instance_)
      return *instance_;
    else
      return *(instance_ = new ObjectIconColumns());
  }

private:
  static ObjectIconColumns* instance_;

  ObjectIconColumns() {
    add(pathname); 
    add(url);
    add(icon);
  }
};

ObjectIconColumns* ObjectIconColumns::instance_ = 0;

ObjectSelector::ObjectSelector(EditorWindow& editor_)
  : editor(editor_),
    label("Object Selector", Gtk::ALIGN_LEFT)
{
  Glib::RefPtr<Gtk::UIManager>   ui_manager   = editor.get_ui_manager();
  Glib::RefPtr<Gtk::ActionGroup> action_group = Gtk::ActionGroup::create();

  action_group->add(Gtk::Action::create("ObjectSelectorRefresh", Gtk::Stock::REFRESH),
                    sigc::mem_fun(*this, &ObjectSelector::refresh));

  ui_manager->insert_action_group(action_group);

  ui_manager->add_ui_from_string("<ui>"
                                 "  <toolbar name='ObjectSelectorToolBar'>"
                                 "    <toolitem action='ObjectSelectorRefresh'/>"
                                 "  </toolbar>"
                                 "</ui>");

  Gtk::Toolbar& toolbar = dynamic_cast<Gtk::Toolbar&>(*ui_manager->get_widget("/ObjectSelectorToolBar"));
  
  list_store = Gtk::ListStore::create(ObjectIconColumns::instance());

  // Change background color
  // iconview.modify_base(Gtk::STATE_NORMAL, Gdk::Color("#444444"));
  
  set_border_width(1);

  iconview.set_model(list_store);

  // Trying to get spacing to minimum, without much effect
  iconview.set_spacing(0);
  iconview.set_column_spacing(0);
  iconview.set_row_spacing(0);

  iconview.set_pixbuf_column(ObjectIconColumns::instance().icon);
  //iconview.set_text_column(ObjectIconColumns::instance().pathname);

  iconview.set_model(list_store);

  std::vector<Gtk::TargetEntry> targets;
  //targets.push_back(Gtk::TargetEntry("image/png"));
  targets.push_back(Gtk::TargetEntry("text/uri-list"));
  targets.push_back(Gtk::TargetEntry("STRING"));
  targets.push_back(Gtk::TargetEntry("text/plain"));
  targets.push_back(Gtk::TargetEntry("application/x-windstille-decal"));
  iconview.drag_source_set(targets, Gdk::BUTTON1_MASK, Gdk::ACTION_COPY);

  iconview.signal_drag_begin().connect(sigc::mem_fun(*this, &ObjectSelector::on_drag_begin));
  iconview.signal_drag_data_get().connect(sigc::mem_fun(*this, &ObjectSelector::on_drag_data_get));

  scrolled.set_policy(Gtk::POLICY_NEVER, Gtk::POLICY_ALWAYS);

  scrolled.add(iconview);
  pack_start(label, Gtk::PACK_SHRINK);

  pack_start(toolbar, Gtk::PACK_SHRINK);
  add(scrolled);
  //show_all();
}

ObjectSelector::~ObjectSelector()
{

}

void
ObjectSelector::add_decal(const Glib::RefPtr<Gdk::Pixbuf>& icon,
                          const std::string& pathname,
                          const std::string& url)
{
  Gtk::ListStore::iterator it = list_store->append();

  (*it)[ObjectIconColumns::instance().pathname] = pathname;
  (*it)[ObjectIconColumns::instance().url]      = url;
  (*it)[ObjectIconColumns::instance().icon]     = icon;
}

static bool has_suffix(const std::string& str, const std::string& suffix)
{
  if (str.length() >= suffix.length())
    return str.compare(str.length() - suffix.length(), suffix.length(), suffix) == 0;
  else
    return false;
}

void
ObjectSelector::add_decals_from_directory(const std::string& pathname)
{
  std::vector<Glib::ustring> images;

  Glib::Dir dir(pathname);
  for(Glib::Dir::iterator i = dir.begin(); i != dir.end(); ++i)
    {
      if (has_suffix(*i, ".png"))
        {
          Glib::ustring path = pathname;
          path += *i;
          images.push_back(path);
        }
    }

  std::sort(images.begin(), images.end());

  for(std::vector<Glib::ustring>::iterator i = images.begin(); i != images.end(); ++i)    
    {
      Glib::RefPtr<Gdk::Pixbuf> pixbuf = Gdk::Pixbuf::create_from_file(*i);
      Glib::RefPtr<Gdk::Pixbuf> icon;

      { // Create an icon, by scaling it down, keeping aspect
        // intact and adding a background (important to make drag&drop easier)
        int size     = 48; // size of the icon
        int min_size = 16; // minimum width/height of the icon after scaling

        if (1)
          {
            icon = Gdk::Pixbuf::create_from_file("data/editor/icon_bg.png");
            size = std::max(icon->get_width(), icon->get_height());
          }
        else
          {
            icon = Gdk::Pixbuf::create(Gdk::COLORSPACE_RGB, true, 8, size, size);
            icon->fill(0x444444ff);
          }

        // Scale pixbuf to icon size while keeping aspect ratio intact
        double x_scale = (double)size / pixbuf->get_width();
        double y_scale = (double)size / pixbuf->get_height();

        if (x_scale * pixbuf->get_width() < min_size)
          x_scale = min_size / pixbuf->get_width();

        if (y_scale * pixbuf->get_height() < min_size)
          y_scale = min_size / pixbuf->get_height();

        if (pixbuf->get_width() > pixbuf->get_height())              
          y_scale = x_scale;
        else
          x_scale = y_scale;

        int r_w = int(pixbuf->get_width() * x_scale);
        int r_h = int(pixbuf->get_height() * y_scale);

        pixbuf->composite(icon, 
                          (size - r_w)/2, (size - r_h)/2,
                          r_w, r_h,
                          (size - r_w)/2, (size - r_h)/2,
                          x_scale, y_scale,
                          Gdk::INTERP_TILES, 
                          255);
      }

      add_decal(icon, *i, 
                "file:///home/ingo/projects/windstille/trunk/windstille/" + *i);
    }
}

void
ObjectSelector::populate()
{
  add_decals_from_directory("data/images/decal/");
  add_decals_from_directory("data/images/objects/bar/");
  add_decals_from_directory("data/images/objects/");
  add_decals_from_directory("data/images/");
  //("data/images/inventory/");
  //("data/images/portraits/");
}
void
ObjectSelector::refresh()
{
  std::cout << "ObjectSelector::refresh()" << std::endl;
  list_store->clear();
  populate();
}
                    
void
ObjectSelector::on_drag_begin(const Glib::RefPtr<Gdk::DragContext>& context)
{
  std::string iconpath = "data/editor/icon.png";

  Gtk::IconView::ArrayHandle_TreePaths selection = iconview.get_selected_items();
  for(Gtk::IconView::ArrayHandle_TreePaths::iterator i = selection.begin();
      i != selection.end();
      ++i)
    {
      Gtk::ListStore::iterator it = list_store->get_iter(*i);
      iconpath = (*it)[ObjectIconColumns::instance().pathname];
    }

  Glib::RefPtr<Gdk::Pixbuf> pixbuf = Gdk::Pixbuf::create_from_file(iconpath);
  if (WindstilleWidget* wst = EditorWindow::current()->get_windstille_widget())
    {
      pixbuf = pixbuf->scale_simple(std::max(4, int(pixbuf->get_width()  * wst->get_state().get_zoom())),
                                    std::max(4, int(pixbuf->get_height() * wst->get_state().get_zoom())),
                                    Gdk::INTERP_TILES);
    }
  context->set_icon(pixbuf, pixbuf->get_width()/2, pixbuf->get_height()/2);
}     

void
ObjectSelector::on_drag_data_get(const Glib::RefPtr<Gdk::DragContext>& context, 
                                 Gtk::SelectionData& selection_data, 
                                 guint info, guint time)
{
  //std::cout << "ObjectSelector: on_drag_data_get" << std::endl;

  Gtk::IconView::ArrayHandle_TreePaths selection = iconview.get_selected_items();

  for(Gtk::IconView::ArrayHandle_TreePaths::iterator i = selection.begin();
      i != selection.end();
      ++i)
    {
      Gtk::ListStore::iterator it = list_store->get_iter(*i);

      std::cout << "TARGET: " << selection_data.get_target() << std::endl;

      if (selection_data.get_target() == "application/x-windstille-decal")
        {
          const std::string& str = (*it)[ObjectIconColumns::instance().pathname];
          selection_data.set(8, reinterpret_cast<const guint8*>(str.c_str()), str.length());
        }
      else
        {
          const std::string& str = (*it)[ObjectIconColumns::instance().url];
          selection_data.set(8, reinterpret_cast<const guint8*>(str.c_str()), str.length());
        }
    }
}

/* EOF */
