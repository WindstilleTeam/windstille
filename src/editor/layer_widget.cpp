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
#include <gtkmm/togglebutton.h>
#include <gtkmm/separator.h>

#include "editor/select_mask.hpp"
#include "editor/layer_widget.hpp"

LayerWidget::LayerWidget()
  : table(2, 9, false),
    buttons(),
    signal_layer_toggle()
{
  int layer_number = 0;
  for(int y = 0; y < 2; ++y)
    for(int x = 0; x < 9; ++x)
    {
      if ((x+1) % 5)
      {
	Gtk::ToggleButton* button = Gtk::manage(new Gtk::ToggleButton());
	button->set_size_request(16, 16);
	table.attach(*button, x, x+1, y, y+1);

	button->signal_toggled().connect(sigc::bind(sigc::mem_fun(*this, &LayerWidget::on_layer_toggle), 
						    button, layer_number));
	buttons.push_back(button);
	layer_number += 1;
      }
      else
      {
	if (y == 0)
	{
	  Gtk::VSeparator* separator = Gtk::manage(new Gtk::VSeparator());
	  //Gtk::Widget* separator = Gtk::manage(new Gtk::Widget());
	  separator->set_size_request(12, -1);
	  table.attach(*separator, x, x+1, 0, 2);
	}
      }
    }

  add(table);
}

LayerWidget::~LayerWidget()
{  
}

void
LayerWidget::on_layer_toggle(Gtk::ToggleButton* button, int layer)
{
  signal_layer_toggle(layer, button->get_active());
}

void
LayerWidget::update(const SelectMask& layers)
{
  for(int i = 0; i < layers.size(); ++i)
  {
    buttons[i]->set_active(layers.get(i));
  }
}

SelectMask
LayerWidget::get_select_mask() const
{
  SelectMask select_mask;
  for(int i = 0; i < select_mask.size(); ++i)
  {
    select_mask.set(i, buttons[i]->get_active());
  }
  return select_mask;
}

/* EOF */
