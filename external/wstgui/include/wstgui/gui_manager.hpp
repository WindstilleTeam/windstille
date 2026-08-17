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

#ifndef HEADER_WINDSTILLE_GUI_GUI_MANAGER_HPP
#define HEADER_WINDSTILLE_GUI_GUI_MANAGER_HPP

#include <memory>

#include <wstdisplay/graphics_context.hpp>
#include <wstinput/controller.hpp>

#include "fwd.hpp"
#include "screen.hpp"

namespace wstgui {

class GUIManager : public Screen
{
public:
  GUIManager(Style& style);
  ~GUIManager() override;

  void draw(wstdisplay::GraphicsContext& gc) override;
  void update(float delta, const wstinput::Controller& controller) override;

  RootComponent* get_root() const;
  Style& get_style() const { return m_style; }

private:
  std::unique_ptr<RootComponent> m_root;
  Style& m_style;
  Component* m_focus_component;

private:
  GUIManager (const GUIManager&);
  GUIManager& operator= (const GUIManager&);
};

} // namespace wstgui

#endif

/* EOF */
