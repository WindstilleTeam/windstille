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
#include <gtkmm/separatortoolitem.h>
#include <gtkmm/stock.h>
#include <gtkmm/image.h>
#include <gtkmm/toolbar.h>
#include <gtkmm/treemodelcolumn.h>

#include "editor/windstille_widget.hpp"
#include "editor/editor_window.hpp"
#include "editor/object_selector.hpp"

class ObjectSelector::Columns : public Gtk::TreeModel::ColumnRecord
{
public:
  Gtk::TreeModelColumn<std::string> url;
  Gtk::TreeModelColumn<std::string> pathname;
  Gtk::TreeModelColumn<Glib::RefPtr<Gdk::Pixbuf> > icon;
  Gtk::TreeModelColumn<unsigned int>    filter_mask;
  
  static ObjectSelector::Columns& instance() {
    if (instance_)
      return *instance_;
    else
      return *(instance_ = new ObjectSelector::Columns());
  }

private:
  static ObjectSelector::Columns* instance_;

  Columns() 
    : url(),
      pathname(),
      icon(),
      filter_mask()
  {
    add(pathname); 
    add(url);
    add(icon);
    add(filter_mask);
  }
};

ObjectSelector::Columns* ObjectSelector::Columns::instance_ = 0;

ObjectSelector::ObjectSelector(EditorWindow& editor_)
  : editor(editor_),
    label("Object Selector", Gtk::ALIGN_LEFT),
    scrolled(),
    iconview(),
    list_store(),
    list_filter(),
    filter_box(),
    filter_entries(),
    filter_mask(OBJECT_GROUP_ALL)
{
#if 0
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
#endif

  Gtk::Button& refresh_button = *Gtk::manage(new Gtk::Button());
  refresh_button.add(*Gtk::manage(new Gtk::Image(Gtk::Stock::REFRESH, Gtk::ICON_SIZE_MENU)));
  refresh_button.set_relief(Gtk::RELIEF_NONE);
  refresh_button.signal_clicked().connect(sigc::mem_fun(*this, &ObjectSelector::refresh));

  filter_entries.push_back(ComboBoxEntry("All", OBJECT_GROUP_ALL));
  filter_entries.push_back(ComboBoxEntry("Walls & Ground", OBJECT_GROUP_DECAL | OBJECT_GROUP_WALL | OBJECT_GROUP_GROUND));
  filter_entries.push_back(ComboBoxEntry("Decor", OBJECT_GROUP_DECAL | OBJECT_GROUP_DECOR));
  filter_entries.push_back(ComboBoxEntry("Lights", OBJECT_GROUP_LIGHT));
  filter_entries.push_back(ComboBoxEntry("Highlights", OBJECT_GROUP_HIGHLIGHT));
  filter_entries.push_back(ComboBoxEntry("Particle Systems", OBJECT_GROUP_PARTICLESYSTEM));

  for(std::vector<ComboBoxEntry>::const_iterator i = filter_entries.begin(); i != filter_entries.end(); ++i)
  {
    filter_box.append_text(i->name);
  }

  filter_box.set_active(0);
  filter_box.signal_changed().connect(sigc::mem_fun(*this, &ObjectSelector::on_filter_changed));

  list_store  = Gtk::ListStore::create(Columns::instance());
  list_filter = Gtk::TreeModelFilter::create(list_store);
  list_filter->set_visible_func(sigc::mem_fun(*this, &ObjectSelector::filter));
  iconview.set_model(list_filter);
  
  // Change background color
  // iconview.modify_base(Gtk::STATE_NORMAL, Gdk::Color("#444444"));

  // Trying to get spacing to minimum, without much effect
  iconview.set_spacing(0);
  iconview.set_column_spacing(0);
  iconview.set_row_spacing(0);

  iconview.set_pixbuf_column(Columns::instance().icon);
  //iconview.set_text_column(Columns::instance().pathname);

  std::vector<Gtk::TargetEntry> targets;
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

  Gtk::HBox& hbox = *Gtk::manage(new Gtk::HBox());
  //hbox.pack_start(toolbar,   Gtk::PACK_SHRINK);
  hbox.pack_start(refresh_button,   Gtk::PACK_SHRINK);
  hbox.pack_start(filter_box, Gtk::PACK_SHRINK);
  pack_start(hbox, Gtk::PACK_SHRINK);
  
  add(scrolled);
  //show_all();
}

ObjectSelector::~ObjectSelector()
{

}

void
ObjectSelector::add_decal(const Glib::RefPtr<Gdk::Pixbuf>& icon,
                          const std::string& pathname,
                          const std::string& url,
                          unsigned int filter_)
{
  Gtk::ListStore::iterator it = list_store->append();

  (*it)[Columns::instance().pathname]    = pathname;
  (*it)[Columns::instance().url]         = url;
  (*it)[Columns::instance().icon]        = icon;
  (*it)[Columns::instance().filter_mask] = filter_;
}

static bool has_suffix(const std::string& str, const std::string& suffix)
{
  if (str.length() >= suffix.length())
    return str.compare(str.length() - suffix.length(), suffix.length(), suffix) == 0;
  else
    return false;
}

bool
ObjectSelector::filter(const Gtk::TreeModel::const_iterator& it)
{
  return (*it)[Columns::instance().filter_mask] & filter_mask;
}

void
ObjectSelector::add_decals_from_directory(const std::string& pathname, unsigned int filter_)
{
  std::vector<Glib::ustring> images;

  Glib::Dir dir(pathname);
  for(Glib::Dir::iterator i = dir.begin(); i != dir.end(); ++i)
  {
    if (has_suffix(*i, ".png"))
    {
      Glib::ustring path_ = pathname;
      path_ += *i;
      images.push_back(path_);
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
              "file:///home/ingo/projects/windstille/trunk/windstille/" + *i, 
              filter_);
  }
}

void
ObjectSelector::populate()
{
  add_decals_from_directory("data/images/decal/", OBJECT_GROUP_DECAL);
  add_decals_from_directory("data/images/objects/bar/", OBJECT_GROUP_DECAL | OBJECT_GROUP_OBJECT | OBJECT_GROUP_DECOR);
  add_decals_from_directory("data/images/objects/", OBJECT_GROUP_DECAL | OBJECT_GROUP_OBJECT | OBJECT_GROUP_DECOR);
  add_decals_from_directory("data/images/", OBJECT_GROUP_LIGHT);
  //("data/images/inventory/");
  //("data/images/portraits/");
}
void
ObjectSelector::refresh()
{
  list_store->clear();
  populate();
}

void
ObjectSelector::on_filter_changed()
{
  filter_mask = filter_entries[filter_box.get_active_row_number()].filter_mask;
  list_filter->refilter();
}
                    
void
ObjectSelector::on_drag_begin(const Glib::RefPtr<Gdk::DragContext>& context)
{
  std::string iconpath;

  Gtk::IconView::ArrayHandle_TreePaths selection = iconview.get_selected_items();
  for(Gtk::IconView::ArrayHandle_TreePaths::iterator i = selection.begin();
      i != selection.end();
      ++i)
  {
    Gtk::TreeModel::Path path_ = list_filter->convert_path_to_child_path(*i);
    Gtk::ListStore::iterator it = list_store->get_iter(path_);

    iconpath = (*it)[Columns::instance().pathname];
  }

  if (iconpath.empty())
  {
    // refuse drag: how?
  }
  else
  {
    Glib::RefPtr<Gdk::Pixbuf> pixbuf = Gdk::Pixbuf::create_from_file(iconpath);
    if (WindstilleWidget* wst = EditorWindow::current()->get_windstille_widget())
    {
      pixbuf = pixbuf->scale_simple(std::max(4, int(static_cast<float>(pixbuf->get_width())  * wst->get_state().get_zoom())),
                                    std::max(4, int(static_cast<float>(pixbuf->get_height()) * wst->get_state().get_zoom())),
                                    Gdk::INTERP_TILES);
    }
    context->set_icon(pixbuf, pixbuf->get_width()/2, pixbuf->get_height()/2);
  }
}

void
ObjectSelector::on_drag_data_get(const Glib::RefPtr<Gdk::DragContext>& /*context*/,
                                 Gtk::SelectionData& selection_data, 
                                 guint /*info*/, guint /*time*/)
{
  //std::cout << "ObjectSelector: on_drag_data_get" << std::endl;

  const Gtk::IconView::ArrayHandle_TreePaths& selection = iconview.get_selected_items();

  for(Gtk::IconView::ArrayHandle_TreePaths::const_iterator i = selection.begin();
      i != selection.end();
      ++i)
  {
    Gtk::TreeModel::Path path_ = list_filter->convert_path_to_child_path(*i);
    Gtk::ListStore::iterator it = list_store->get_iter(path_);

    if (selection_data.get_target() == "application/x-windstille-decal")
    {
      const std::string& str = (*it)[Columns::instance().pathname];
      selection_data.set(8, reinterpret_cast<const guint8*>(str.c_str()), str.length());
    }
    else
    {
      const std::string& str = (*it)[Columns::instance().url];
      selection_data.set(8, reinterpret_cast<const guint8*>(str.c_str()), str.length());
    }
  }
}

/* EOF */
