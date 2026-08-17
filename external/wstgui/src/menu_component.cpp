/*
**  Windstille - A Sci-Fi Action-Adventure Game
**  Copyright (C) 2005 Ingo Ruhnke <grumbel@gmail.com>
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

#include "menu_component.hpp"

#include <wstinput/controller.hpp>
#include <wstdisplay/graphics_context.hpp>

#include "controller_def.hpp"
#include "menu_item.hpp"
#include "tab_component.hpp"

namespace wstgui {

MenuComponent::MenuComponent(bool allow_cancel_, Component* parent_) :
  Component(parent_),
  m_items(),
  m_current_item(0),
  m_font(get_style().get_font()),
  m_allow_cancel(allow_cancel_),
  m_scroll_mode(false),
  m_scroll_offset(0),
  m_num_displayable_items(-1)
{
}

MenuComponent::~MenuComponent()
{
}

void
MenuComponent::add_item(std::unique_ptr<MenuItem> item)
{
  m_items.emplace_back(std::move(item));

  if (calc_height() >= m_geometry.height())
  {
    m_scroll_mode   = true;
    m_scroll_offset = 0;
  }
}

void
MenuComponent::draw(wstdisplay::GraphicsContext& gc)
{
  float step = item_height();

  if (m_scroll_mode)
  { // we can only display a subset of items and have to scroll
    for(int i = 0; i < m_num_displayable_items; ++i)
    {
      m_items[i + m_scroll_offset]->draw(gc,
                                         geom::frect(m_geometry.left(), m_geometry.top() + static_cast<float>(i) * step + 2.0f,
                                                     m_geometry.right() - 32.0f, m_geometry.top() + static_cast<float>(i+1) * step - 2.0f),
                                         is_active() && (i + m_scroll_offset == m_current_item));
    }

    // draw scrollbar
    float scrollbar_height = (m_geometry.height() - 4.0f) * static_cast<float>(m_num_displayable_items) / static_cast<float>(m_items.size());
    float scrollbar_incr   = (m_geometry.height() - 4.0f) * static_cast<float>(m_scroll_offset) / static_cast<float>(m_items.size());

    gc.fill_rounded_rect(geom::frect(m_geometry.right() - 24, m_geometry.top() + 2.0f + scrollbar_incr,
                                     m_geometry.right() - 2,  m_geometry.top() + 2.0f + scrollbar_incr + scrollbar_height),
                         5.0f,
                         surf::Color(0.5f, 0.5f, 0.5f, 0.75f));

    gc.draw_rounded_rect(geom::frect(m_geometry.right() - 24, m_geometry.top() + 2.0f,
                                     m_geometry.right() - 2,  m_geometry.bottom() - 2.0f),
                         5.0f,
                         surf::Color(1.0f, 1.0f, 1.0f, 1.0f));
  }
  else
  { // all items fit on the screen
    for(size_t i = 0; i < m_items.size(); ++i)
    {
      m_items[i]->draw(gc,
                       geom::frect(m_geometry.left(), m_geometry.top() + static_cast<float>(i) * step + 2.0f,
                                   m_geometry.right(), m_geometry.top() + static_cast<float>(i+1) * step - 2.0f),
                       is_active() && (int(i) == m_current_item));
    }
  }
}

void
MenuComponent::update(float delta, const Controller& controller)
{
  for(size_t i = 0; i < m_items.size(); ++i)
  {
    m_items[i]->update(delta);
  }

  for(auto i = controller.get_events().begin();
      i != controller.get_events().end();
      ++i)
  {
    if (i->type == wstinput::BUTTON_EVENT && i->button.down)
    {
      if (i->button.name == OK_BUTTON || i->button.name == ENTER_BUTTON)
      {
        m_items[m_current_item]->click();
      }
      else if (i->button.name == CANCEL_BUTTON ||
               i->button.name == ESCAPE_BUTTON ||
               i->button.name == PAUSE_BUTTON)
      {
        if (m_allow_cancel) // FIXME: Could use a signal instead
        {
            // FIXMESOUND: g_app.sound().play(Pathname("sounds/menu_click.wav", Pathname::kDataPath));
          set_active(false);
        }
      }
      else if (i->button.name == MENU_LEFT_BUTTON)
      {
        m_items[m_current_item]->incr();
      }
      else if (i->button.name == MENU_RIGHT_BUTTON)
      {
        m_items[m_current_item]->decr();
      }
      else if (i->button.name == MENU_UP_BUTTON)
      {
        // FIXMESOUND: g_app.sound().play(Pathname("sounds/menu_change.wav", Pathname::kDataPath));

        m_current_item = m_current_item - 1;
        if (m_current_item < 0)
        {
          if (dynamic_cast<TabComponent*>(m_parent))
          {
            m_current_item = 0;
            set_active(false);
          }
          else
          {
            m_current_item = static_cast<int>(m_items.size())-1;
          }
        }

        adjust_scroll_offset();
      }
      else if (i->button.name == MENU_DOWN_BUTTON)
      {
        // FIXMESOUND: g_app.sound().play(Pathname("sounds/menu_change.wav", Pathname::kDataPath));

        if (dynamic_cast<TabComponent*>(m_parent))
        {
          m_current_item = std::clamp(m_current_item + 1, 0, static_cast<int>(m_items.size()-1));
        }
        else
        {
          m_current_item += 1;
          if (m_current_item >= static_cast<int>(m_items.size()))
          {
            m_current_item = 0;
          }

        }

        adjust_scroll_offset();
      }
    }
  }
}

void
MenuComponent::set_font(wstdisplay::TTFFont* font_)
{
  m_font = font_;
}

wstdisplay::TTFFont*
MenuComponent::get_font()
{
  return m_font;
}

geom::fsize
MenuComponent::get_prefered_size() const
{
  /*
    float width = 0;
    for(Items::iterator i = items.begin(); i != items.end(); ++i)
    {
    width = std::max(get_width())
    }  */
  return geom::fsize(m_geometry.width(),
                     item_height() * static_cast<float>(std::min(10, int(m_items.size()))) + 12.0f);
}

float
MenuComponent::calc_height()
{
  return static_cast<float>(m_items.size()) * item_height();
}

float
MenuComponent::item_height() const
{
  float spacing = 10.0f;
  return static_cast<float>(m_font->get_height()) + spacing * 2.0f;
}

void
MenuComponent::adjust_scroll_offset()
{
  if (m_scroll_mode)
  {
    if (m_current_item - m_scroll_offset >= m_num_displayable_items)
    { // scrolling down
      m_scroll_offset = m_current_item - (m_num_displayable_items - 1);
    }
    else if (m_current_item < m_scroll_offset)
    { // scrolling up
      m_scroll_offset = m_current_item;
    }
  }
}

void
MenuComponent::set_geometry(const geom::frect& rect)
{
  m_num_displayable_items = static_cast<int>(rect.height() / item_height());

  if (m_num_displayable_items < int(m_items.size()))
  {
    m_scroll_mode   = true;
    m_scroll_offset = 0;
  }
  else
  {
    m_scroll_mode   = false;
    m_scroll_offset = 0;
  }

  Component::set_geometry(rect);
}

} // namespace wstgui

/* EOF */
