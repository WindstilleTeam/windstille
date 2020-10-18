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

#include "gui/grid_component.hpp"

#include <iostream>

#include <wstinput/controller.hpp>

#include "app/controller_def.hpp"
#include "display/color.hpp"
#include "display/graphics_context.hpp"
#include "util/log.hpp"

namespace gui {

GridComponent::GridComponent(Component* parent_)
  : Component(parent_),
    grid(),
    pos(0, 0),
    child_active(),
    padding()
{
  log_error("not implemented");
}

GridComponent::GridComponent(const geom::frect& rect_, int weight, int height, Component* parent_)
  : Component(rect_, parent_),
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
GridComponent::draw(GraphicsContext& gc)
{
  gc.fill_rect(rect, RGBAf(0.0f, 0.0f, 0.0f, 0.5f));
  gc.draw_rect(rect, RGBAf(1.0f, 1.0f, 1.0f, 0.5f));

  for(int y = 0; y < grid.get_height(); ++y)
    for(int x = 0; x < grid.get_width(); ++x)
    {
      if (grid(x, y).component && !grid(x, y).has_parent())
      {
        if (x == pos.x && y == pos.y)
          gc.fill_rect(grid(x, y).component->get_screen_rect(), RGBAf(1.0f, 1.0f, 1.0f, 0.5f));

        grid(x, y).component->draw(gc);
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
    for(auto i = controller.get_events().begin();
        i != controller.get_events().end();
        ++i)
    {
      if (i->type == wstinput::BUTTON_EVENT && i->button.down)
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
      else if (i->type == wstinput::AXIS_EVENT)
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

  pos.y += grid(pos.x, pos.y).span.height();
  if (pos.y >= grid.get_height())
    pos.y = 0;

  if (grid(pos.x, pos.y).has_parent())
    pos = grid(pos.x, pos.y).parent.as_vec();

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
    pos = grid(pos.x, pos.y).parent.as_vec();

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
    pos = grid(pos.x, pos.y).parent.as_vec();

  //grid(pos.x, pos.y).component->set_active(true);
}

void
GridComponent::move_right()
{
  //grid(pos.x, pos.y).component->set_active(false);

  pos.x += grid(pos.x, pos.y).span.width();
  if (pos.x >= grid.get_width())
    pos.x = 0;

  if (grid(pos.x, pos.y).has_parent())
    pos = grid(pos.x, pos.y).parent.as_vec();

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
    geom::frect rect_ = get_screen_rect();

    if (colspan == 1 && rowspan == 1)
    {
      grid(x, y) = ComponentBox(component, geom::isize(colspan, rowspan));
    }
    else
    {
      for(int iy = 0; iy < rowspan; ++iy)
        for(int ix = 0; ix < colspan; ++ix)
        {
          grid(x + ix, y + iy) = ComponentBox(component, geom::isize(0, 0), geom::ipoint(x, y));
        }
      grid(x, y) = ComponentBox(component, geom::isize(colspan, rowspan));
    }

    component->set_screen_rect(geom::frect(glm::vec2(rect_.left() + static_cast<float>(x) * (rect_.width()  / static_cast<float>(grid.get_width()))  + padding,
                                              rect_.top()  + static_cast<float>(y) * (rect_.height() / static_cast<float>(grid.get_height())) + padding),
                                     geom::fsize((rect_.width()  / static_cast<float>(grid.get_width()))  * static_cast<float>(colspan) - 2.0f * padding,
                                           (rect_.height() / static_cast<float>(grid.get_height())) * static_cast<float>(rowspan) - 2.0f * padding)));
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
