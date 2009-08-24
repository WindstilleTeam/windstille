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
#include "display/display.hpp"
#include "input/controller.hpp"
#include "grid_component.hpp"

namespace gui {

GridComponent::GridComponent(Component* parent)
  : Component(parent),
    grid(),
    pos(),
    child_active(),
    padding()
{
  assert(0);  
}

GridComponent::GridComponent(const Rectf& rect, int weight, int height, Component* parent)
  : Component(rect, parent),
    grid(weight, height),
    pos(0, 0),
    child_active(false),
    padding(15.0f)
{
}

GridComponent::~GridComponent()
{
  for(Grid::iterator i = grid.begin(); i != grid.end(); ++i)
    delete i->component;
  grid.clear();
}

void
GridComponent::draw()
{
  Display::fill_rect(rect, Color(0.0f, 0.0f, 0.0f, 0.5f));
  Display::draw_rect(rect, Color(1.0f, 1.0f, 1.0f, 0.5f));

  for(int y = 0; y < grid.get_height(); ++y)
    for(int x = 0; x < grid.get_width(); ++x)
    {
      if (grid(x, y).component && !grid(x, y).has_parent())
      {
        if (x == pos.x && y == pos.y)
          Display::fill_rect(grid(x, y).component->get_screen_rect(), Color(1.0f, 1.0f, 1.0f, 0.5f));
            
        grid(x, y).component->draw();
      }
    }
}

void
GridComponent::update(float delta, const Controller& controller)
{
  if (child_active && !grid(pos.x, pos.y).component->is_active())
  {
    child_active = false;
  }
  else if (!child_active)
  {
    for(InputEventLst::const_iterator i = controller.get_events().begin(); 
        i != controller.get_events().end(); 
        ++i)
    {
      if (i->type == BUTTON_EVENT && i->button.down)
      {
        if (i->button.name == OK_BUTTON)
        {
          child_active = true;
          grid(pos.x, pos.y).component->set_active(true);
        }
        else if (i->button.name == CANCEL_BUTTON)
        {
          set_active(false);
        }
      }
      else if (i->type == AXIS_EVENT)
      {
        if (i->axis.name == X_AXIS)
        {
          if (i->axis.pos < 0)
          {
            move_left();
          }
          else if (i->axis.pos > 0)
          {
            move_right();
          }
        }
        else if (i->axis.name == Y_AXIS)
        {
          if (i->axis.pos < 0)
          {
            move_down();
          }
          else if (i->axis.pos > 0)
          {
            move_up();
          }
        }
      }
    }
  }

  for(int y = 0; y < grid.get_height(); ++y)
    for(int x = 0; x < grid.get_width(); ++x)
    {
      if (grid(x, y).component && !grid(x, y).has_parent())
      {
        // give input to current compontent, empty input to the rest
        if (child_active && pos.x == x && pos.y == y)
          grid(x, y).component->update(delta, controller);
        else
          grid(x, y).component->update(delta, Controller());
      }
    }
}

void
GridComponent::move_up()
{
  //grid(pos.x, pos.y).component->set_active(false);

  pos.y += grid(pos.x, pos.y).span.height;
  if (pos.y >= grid.get_height())
    pos.y = 0;

  if (grid(pos.x, pos.y).has_parent())
    pos = grid(pos.x, pos.y).parent;

  //grid(pos.x, pos.y).component->set_active(true);
}

void
GridComponent::move_down()
{
  //grid(pos.x, pos.y).component->set_active(false);

  pos.y -= 1;
  if (pos.y < 0)
    pos.y = grid.get_height()-1;

  if (grid(pos.x, pos.y).has_parent())
    pos = grid(pos.x, pos.y).parent;

  //grid(pos.x, pos.y).component->set_active(true);
}

void
GridComponent::move_left()
{
  //grid(pos.x, pos.y).component->set_active(false);

  pos.x -= 1;
  if (pos.x < 0)
    pos.x = grid.get_width()-1;  

  if (grid(pos.x, pos.y).has_parent())
    pos = grid(pos.x, pos.y).parent;

  //grid(pos.x, pos.y).component->set_active(true);
}

void
GridComponent::move_right()
{
  //grid(pos.x, pos.y).component->set_active(false);

  pos.x += grid(pos.x, pos.y).span.width;
  if (pos.x >= grid.get_width())
    pos.x = 0;

  if (grid(pos.x, pos.y).has_parent())
    pos = grid(pos.x, pos.y).parent;

  //grid(pos.x, pos.y).component->set_active(true);
}

void
GridComponent::pack(Component* component, int x, int y, int colspan, int rowspan)
{
  assert(x >= 0);
  assert(y >= 0);
  assert(x < grid.get_width());
  assert(y < grid.get_height());

  if (grid(x, y).component)
  {
    std::cout << "Warning component already at: " << x << ", " << y << ", ignoring" << std::endl;
    delete component;
  }
  else
  {
    Rectf rect = get_screen_rect();

    if (colspan == 1 && rowspan == 1)
    {
      grid(x, y) = ComponentBox(component, Size(colspan, rowspan));
    }
    else
    {
      for(int iy = 0; iy < rowspan; ++iy)
        for(int ix = 0; ix < colspan; ++ix)
        {
          grid(x + ix, y + iy) = ComponentBox(component, Size(0, 0), Point(x, y));
        }
      grid(x, y) = ComponentBox(component, Size(colspan, rowspan));
    }

    component->set_screen_rect(Rectf(Vector2f(rect.left + static_cast<float>(x) * (rect.get_width()  / static_cast<float>(grid.get_width()))  + padding,
                                              rect.top  + static_cast<float>(y) * (rect.get_height() / static_cast<float>(grid.get_height())) + padding),
                                     Sizef((rect.get_width()  / static_cast<float>(grid.get_width()))  * static_cast<float>(colspan) - 2.0f * padding,
                                           (rect.get_height() / static_cast<float>(grid.get_height())) * static_cast<float>(rowspan) - 2.0f * padding)));
  }
}

void
GridComponent::set_padding(float p)
{
  padding = p;
}

void
GridComponent::on_activation()
{
  //grid(pos.x, pos.y).component->set_active(true);
}

} // namespace gui

/* EOF */
