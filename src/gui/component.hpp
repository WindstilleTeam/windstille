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

#ifndef HEADER_WINDSTILLE_GUI_COMPONENT_HPP
#define HEADER_WINDSTILLE_GUI_COMPONENT_HPP

#include "math/rect.hpp"

class FileReader;
class Controller;

namespace gui {

/** */
class Component
{
protected:
  Rectf rect;
  Component* parent;
  bool active;

public:
  Component(Component* parent);
  Component(const Rectf& rect, Component* parent);
  virtual ~Component();

  virtual bool is_active() const;
  virtual void set_active(bool a);

  virtual void on_activation() {}
  virtual void draw() =0;
  virtual void update(float delta, const Controller& controller) =0;

  virtual Rectf get_screen_rect() const;
  virtual void  set_screen_rect(const Rectf& rect);

  virtual float get_prefered_width() const;
  virtual float get_prefered_height() const;

private:
  Component (const Component&);
  Component& operator= (const Component&);
};

} // namespace gui

#endif

/* EOF */
