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

#include "timeline_layer_widget.hpp"

#include "timeline_layer.hpp"

namespace windstille {

TimelineLayerWidget::TimelineLayerWidget() :
  m_timeline(),
  m_column_height(32)
{
}

bool
TimelineLayerWidget::on_draw(Cairo::RefPtr<Cairo::Context> const& cr)
{
  if (!m_timeline)
    return false;

  Gtk::Allocation allocation = get_allocation();

  if (1) { // pixel perfect drawing
    cr->translate(0.5, 0.5);
  }

  cr->select_font_face ("Sans", Cairo::FONT_SLANT_NORMAL, Cairo::FONT_WEIGHT_NORMAL);
  cr->set_font_size(12);

  cr->set_line_width(1.0);

  Cairo::TextExtents extents;
  for(Timeline::iterator i = m_timeline->begin(); i != m_timeline->end(); ++i)
  {
    cr->get_text_extents((*i)->get_name(), extents);

    cr->set_source_rgb(1,1,1);
    cr->rectangle(0, m_column_height * static_cast<double>(i - m_timeline->begin()),
                  allocation.get_width(), m_column_height);
    cr->fill();

    // draw text
    cr->set_source_rgb(0,0,0);
    cr->move_to(8, 10 + m_column_height * static_cast<double>(i - m_timeline->begin()) - extents.y_bearing);
    cr->show_text((*i)->get_name());

    // draw line
    cr->set_source_rgb(0,0,0);
    cr->move_to(0, m_column_height + m_column_height * static_cast<double>(i - m_timeline->begin()));
    cr->line_to(allocation.get_width(), m_column_height+ m_column_height * static_cast<double>(i - m_timeline->begin()));
    cr->stroke();
  }

  return false;
}

} // namespace windstille

/* EOF */
