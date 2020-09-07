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

#include "hud/inventory.hpp"

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/ext.hpp>

#include "font/fonts.hpp"
#include "input/controller.hpp"
#include "screen/game_session.hpp"

class InventoryItem
{
public:
  std::string name;
  Sprite sprite;

  InventoryItem(const std::string& name_, const std::string& filename_)
    : name(name_), sprite(Pathname(filename_))
  {
  }
};

class InventoryImpl
{
public:
  int   moving;
  float add_angle;
  Sprite slot;
  Sprite slothighlight;
  typedef std::vector<InventoryItem> Items;
  Items items;
  int current_item;

  InventoryImpl()
    : moving(),
      add_angle(),
      slot(),
      slothighlight(),
      items(),
      current_item()
  {}

  void update(float delta, const Controller& controller);
  void draw();

  void incr_current_item() {
    if (current_item == int(items.size()) - 1)
      current_item = 0;
    else
      current_item += 1;
  }

  void decr_current_item() {
    if (current_item == 0)
      current_item = static_cast<int>(items.size()) - 1;
    else
      current_item -= 1;
  }
};

Inventory::Inventory()
  : impl(new InventoryImpl())
{
  impl->slot = Sprite(Pathname("images/inventory/slot.sprite"));
  impl->slothighlight = Sprite(Pathname("images/inventory/slothighlight.sprite"));
  impl->moving = false;
  impl->add_angle = 0.0f;
  impl->current_item = 0;

  impl->items.push_back(InventoryItem("Flashlight",  "images/inventory/flashlight.sprite"));
  impl->items.push_back(InventoryItem("Stone",       "images/inventory/stone.sprite"));
  impl->items.push_back(InventoryItem("PDA",         "images/inventory/pda.sprite"));
  impl->items.push_back(InventoryItem("5x Granates", "images/inventory/granate.sprite"));
  impl->items.push_back(InventoryItem("Lv1 Keycard", "images/inventory/keycard.sprite"));
}

Inventory::~Inventory()
{
}

void
InventoryImpl::draw()
{
  Vector2f pos = Vector2f(400, 300);

  int num_items = static_cast<int>(items.size());
  float step_angle = (2.0f * math::pi) / static_cast<float>(num_items);

  for(int i = 0; i < int(items.size()); ++i)
  {
    const InventoryItem& item = items[(i+current_item) % items.size()];
    Vector2f draw_pos = pos + glm::rotate(Vector2f(128.0f, 0.0f), step_angle * static_cast<float>(i) - math::pi/2.0f + add_angle);

    if (i == 0 && moving == 0)
    {
      slothighlight.draw(draw_pos);
      Fonts::current()->vera20->draw_center(Vector2f(draw_pos.x, draw_pos.y - 64), item.name);
    }
    else
    {
      slot.draw(draw_pos);
    }

    item.sprite.draw(draw_pos - Vector2f(32,32));
  }
}

void
Inventory::draw()
{
  impl->draw();
}

void
Inventory::update(float delta, const Controller& controller)
{
  impl->update(delta, controller);
}

void
InventoryImpl::update(float delta, const Controller& controller)
{
  float step_angle = (2.0f * math::pi) / static_cast<float>(items.size());
  if (fabsf(add_angle) > step_angle)
  {
    if (moving == 1)
      decr_current_item();
    else if (moving == -1)
      incr_current_item();

    moving = 0;
    add_angle = 0;
  }

  if (controller.get_axis_state(X_AXIS) < -0.5f)
  {
    if (moving == 1)
    {
      add_angle = -step_angle + add_angle;
      decr_current_item();
    }

    moving = -1;
  }
  else if (controller.get_axis_state(X_AXIS) > 0.5f)
  {
    if (moving == -1)
    {
      add_angle = step_angle + add_angle;
      incr_current_item();
    }

    moving =  1;
  }

  if (moving == -1)
  {
    add_angle -= 3 * delta;
  }
  else if (moving == 1)
  {
    add_angle += 3 * delta;
  }

  if (moving == 0)
  {
    if (controller.button_was_pressed(OK_BUTTON) ||
        controller.button_was_pressed(CANCEL_BUTTON) ||
        controller.button_was_pressed(INVENTORY_BUTTON))
    {
      GameSession::current()->set_control_state(GameSession::GAME);
    }
  }
}

/* EOF */
