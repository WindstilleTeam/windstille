/*  $Id$
**   __      __ __             ___        __   __ __   __
**  /  \    /  \__| ____    __| _/_______/  |_|__|  | |  |   ____
**  \   \/\/   /  |/    \  / __ |/  ___/\   __\  |  | |  | _/ __ \
**   \        /|  |   |  \/ /_/ |\___ \  |  | |  |  |_|  |_\  ___/
**    \__/\  / |__|___|  /\____ /____  > |__| |__|____/____/\___  >
**         \/          \/      \/    \/                         \/
**  Copyright (C) 2007 Ingo Ruhnke <grumbel@gmx.de>
**
**  This program is free software; you can redistribute it and/or
**  modify it under the terms of the GNU General Public License
**  as published by the Free Software Foundation; either version 2
**  of the License, or (at your option) any later version.
**
**  This program is distributed in the hope that it will be useful,
**  but WITHOUT ANY WARRANTY; without even the implied warranty of
**  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
**  GNU General Public License for more details.
** 
**  You should have received a copy of the GNU General Public License
**  along with this program; if not, write to the Free Software
**  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA
**  02111-1307, USA.
*/

#ifndef HEADER_WINDSTILLE_GUI_MENU_COMPONENT_HPP
#define HEADER_WINDSTILLE_GUI_MENU_COMPONENT_HPP

#include <string>
#include <vector>
#include "font/fonts.hpp"
#include "component.hpp"

namespace GUI {

class MenuComponent;

class MenuItem {
public:
  MenuComponent* parent;
  std::string label;

  MenuItem(MenuComponent* parent_, const std::string& label_);
  virtual ~MenuItem() {}
  virtual void incr() =0;
  virtual void decr() =0;
  virtual void draw(const Rectf& rect, bool is_active);
  virtual void update(float delta);
};

class BoolMenuItem : public MenuItem {
private: // FIXME: Convert this into a generic enum/value slider
  bool value;
  std::string true_label;
  std::string false_label;

public:  
  BoolMenuItem(MenuComponent* parent_, 
               const std::string& label_, bool value_, 
               const std::string& true_label_ = "on", const std::string& false_label_ = "off");
  void incr();
  void decr();
  void draw(const Rectf& rect, bool is_active);
  void update(float);
};

/** A slider widget for use in volume controls, gamma controls and
    things like that */
class SliderMenuItem : public MenuItem {
public:
  int value;
  int min_value;
  int max_value;
  int step;

public:  
  SliderMenuItem(MenuComponent* parent_, 
                 const std::string& label_, int value_, int mix_value_ = 0, int max_value_ = 100, int step = 10);
  void incr();
  void decr();
  void draw(const Rectf& rect, bool is_active);
  void update(float);
};

/** */
class MenuComponent : public Component
{
private:
  typedef std::vector<MenuItem* > Items;
  Items items;
  int   current_item;
  
public:
  MenuComponent(const Rectf& rect, Component* parent);
  virtual ~MenuComponent();

  void add_item(const std::string& label);
  void add_item(MenuItem* item);
  void draw();
  void update(float delta, const Controller& controller);

private:
  MenuComponent (const MenuComponent&);
  MenuComponent& operator= (const MenuComponent&);
};

} // namespace GUI

#endif

/* EOF */
