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

#include <iostream>
#include <vector>
#include "screen/game_session.hpp"
#include "sprite2d/sprite.hpp"
#include "font/fonts.hpp"
#include "input/input_manager.hpp"
#include "input/controller.hpp"
#include "inventory.hpp"

class InventoryItem
{
public:
  std::string name;
  Sprite sprite;
  
  InventoryItem(const std::string& name, const std::string& filename)
    : name(name), sprite(filename)
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
      current_item = items.size() - 1;
    else
      current_item -= 1;
  }
};

Inventory::Inventory()
  : impl(new InventoryImpl())
{
  impl->slot = Sprite("images/inventory/slot.sprite");
  impl->slothighlight = Sprite("images/inventory/slothighlight.sprite");
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
  Vector2f pos = Vector2f(400, 300); // View::current()->screen_Player::currently->get_pos();

  int num_items = items.size();
  float step_angle = (2*M_PI) / num_items;

  for(int i = 0; i < int(items.size()); ++i)
    {
      const InventoryItem& item = items[(i+current_item)%items.size()];
      Vector2f draw_pos = pos + Vector2f(128, 0).rotate(step_angle * i - M_PI/2 + add_angle);

      if (i == 0 && moving == 0)
        {
          slothighlight.draw(draw_pos);
          Fonts::vera20->draw_center(Vector2f(draw_pos.x, draw_pos.y - 64), item.name);
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
  float step_angle = (2*M_PI) / items.size();
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
      if (controller.button_was_pressed(USE_BUTTON) ||
          controller.button_was_pressed(INVENTORY_BUTTON))
        {
          GameSession::current()->set_control_state(GameSession::GAME);
        }
    }
}

/* EOF */
