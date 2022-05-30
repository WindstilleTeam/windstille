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

#ifndef HEADER_WINDSTILLE_HUD_INVENTORY_HPP
#define HEADER_WINDSTILLE_HUD_INVENTORY_HPP

#include <wstgui/screen.hpp>
#include <memory>

namespace windstille {

class InventoryImpl;

/** */
class Inventory : public wstgui::Screen
{
private:
public:
  Inventory();
  ~Inventory() override;

  void draw(wstdisplay::GraphicsContext& gc) override;
  void update(float delta, wstinput::Controller const& controller) override;

private:
  std::shared_ptr<InventoryImpl> impl;
};

} // namespace windstille

#endif

/* EOF */
