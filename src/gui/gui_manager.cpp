/*
**  Windstille - A Sci-Fi Action-Adventure Game
**  Copyright (C) 2005 Ingo Ruhnke <grumbel@gmx.de>
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

#include "input/controller.hpp"
#include "button.hpp"
#include "slider.hpp"
#include "root_component.hpp"
#include "screen_manager.hpp"
#include "grid_component.hpp"
#include "tab_component.hpp"
#include "list_view.hpp"
#include "text_view.hpp"
#include "automap.hpp"
#include "gui_manager.hpp"

namespace gui {

GUIManager::GUIManager()
{
  root = new RootComponent(Rectf(0,0,800,600));
}

GUIManager::~GUIManager()
{
  delete root;
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
    { //FIXME: This doesn't work when GUI isn't an overlay
      screen_manager.pop_overlay();
    }
}

RootComponent*
GUIManager::get_root() const
{
  return root;
}

} // namespace gui

/* EOF */
