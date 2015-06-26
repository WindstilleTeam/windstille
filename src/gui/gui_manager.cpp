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

#include "gui/root_component.hpp"
#include "screen/screen_manager.hpp"
#include "gui/gui_manager.hpp"

namespace gui {

GUIManager::GUIManager()
  : root(new RootComponent(Rectf(0,0,800,600)))
{
}

GUIManager::~GUIManager()
{
}

void
GUIManager::draw()
{
  root->draw();
}

void
GUIManager::update(float delta, const Controller& controller)
{
  root->update(delta, controller);

  if (!root->is_active())
  {
    //FIXME: This doesn't work when GUI isn't an overlay
    ScreenManager::current()->pop_overlay();
  }
}

RootComponent*
GUIManager::get_root() const
{
  return root.get();
}

} // namespace gui

/* EOF */
