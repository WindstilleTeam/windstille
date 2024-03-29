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

#ifndef HEADER_WINDSTILLE_EDITOR_LAYER_WIDGET_HPP
#define HEADER_WINDSTILLE_EDITOR_LAYER_WIDGET_HPP

#include <gtkmm/toolitem.h>
#include <gtkmm/grid.h>
#include <gtkmm/togglebutton.h>

namespace windstille {

class LayerWidget : public Gtk::ToolItem
{
public:
  LayerWidget();
  ~LayerWidget() override;

  void on_layer_toggle(Gtk::ToggleButton* button, int layer);
  void update(SelectMask const& layers);

  SelectMask get_select_mask() const;

  sigc::signal<void, int, bool> signal_layer_toggle;

private:
  Gtk::Grid m_grid;
  std::vector<Gtk::ToggleButton*> m_buttons;

private:
  LayerWidget(LayerWidget const&);
  LayerWidget& operator=(LayerWidget const&);
};

} // namespace windstille

#endif

/* EOF */
