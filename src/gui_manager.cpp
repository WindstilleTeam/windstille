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

#include "gui_manager.hpp"

#include <logmich/log.hpp>

#include "root_component.hpp"

namespace wstgui {

GUIManager::GUIManager(Style& style) :
  m_root(new RootComponent(style)),
  m_style(style),
  m_focus_component()
{
}

GUIManager::~GUIManager()
{
}

void
GUIManager::draw(wstdisplay::GraphicsContext& gc)
{
  m_root->draw(gc);

  if (m_focus_component) {
    gc.fill_rect(m_focus_component->geometry(), surf::Color(1.0f, 0.0f, 0.0f, 0.5f));
  }
}

void
GUIManager::update(float delta, const Controller& controller)
{
  m_root->update(delta, controller);

  geom::fpoint mouse_pos(controller.get_pointer_state(0), controller.get_pointer_state(1));
  m_focus_component = m_root->query(mouse_pos);

  if (!m_root->is_active()) {
    finish();
  }
}

RootComponent*
GUIManager::get_root() const
{
  return m_root.get();
}

} // namespace wstgui

/* EOF */
