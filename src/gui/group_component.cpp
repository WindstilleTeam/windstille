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

#include <assert.h>
#include "display/display.hpp"
#include "display/color.hpp"
#include "font/fonts.hpp"
#include "group_component.hpp"

namespace gui {

GroupComponent::GroupComponent(const Rectf& rect, const std::string& title_, Component* parent)
  : Component(rect, parent),
    title(title_),
    child()
{
  
}

GroupComponent::~GroupComponent()
{
}

void
GroupComponent::draw()
{
  Display::fill_rounded_rect(rect, 5.0f, Color(0.0f, 0.0f, 0.0f, 0.7));
  Display::draw_rounded_rect(rect, 5.0f, Color(1.0f, 1.0f, 1.0f, 0.5f));

  if (!title.empty())
    {
      TTFFont* font = Fonts::current()->vera20.get();
      font->draw_center(Vector2f(rect.left + rect.get_width()/2, rect.top + font->get_height() + 5), 
                        title, Color(1.0f, 1.0f, 1.0f));

      Display::fill_rect(Rectf(rect.left  + 8, rect.top + font->get_height() + 16,
                               rect.right - 8, rect.top + font->get_height() + 18),
                         Color(1.0f, 1.0f, 1.0f, 0.5f));
    }

  if (child.get())
    child->draw();
}

void
GroupComponent::update(float delta, const Controller& controller)
{
  if (child.get())
    child->update(delta, controller);
}

void
GroupComponent::pack(Component* component)
{
  assert(child.get() == 0);
  child.reset(component);

  child->set_screen_rect(get_child_rect());
  child->set_active(true);
}

Rectf
GroupComponent::get_child_rect() const
{
  int padding = 6;

  return Rectf(rect.left   + padding,
               rect.top    + padding + (title.empty() ? 0 : Fonts::current()->vera20->get_height() + 18),
               rect.right  - padding,
               rect.bottom - padding);
}

bool
GroupComponent::is_active() const
{
  if (child.get())
    return child->is_active();
  else
    return false;
}

} // namespace gui

/* EOF */
