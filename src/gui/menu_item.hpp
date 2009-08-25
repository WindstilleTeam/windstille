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

#ifndef HEADER_WINDSTILLE_GUI_MENU_ITEM_HPP
#define HEADER_WINDSTILLE_GUI_MENU_ITEM_HPP

#include <boost/signal.hpp>

namespace gui {

class MenuComponent;

class MenuItem 
{
protected:
  MenuComponent* parent;
  std::string label;
  float fade_timer;

public:
  MenuItem(MenuComponent* parent_, const std::string& label_);
  virtual ~MenuItem() {}

  virtual void incr() =0;
  virtual void decr() =0;

  virtual void click() =0;

  virtual void draw(const Rectf& rect, bool is_active);
  virtual void update(float delta);
  
private:
  MenuItem(const MenuItem&);
  MenuItem& operator=(const MenuItem&);
};

class EnumMenuItem : public MenuItem 
{
private: // FIXME: Convert this into a generic enum/value slider
  struct EnumValue {
    std::string label;
    int         value;

    EnumValue()
      : label(),
        value()
    {}
  };
  
  int index;
  std::vector<EnumValue> labels;
  boost::signal<void (int)> on_change;
public:  
  EnumMenuItem(MenuComponent* parent_, 
               const std::string& label_, int index_ = 0);
  
  EnumMenuItem& add_pair(int value, const std::string& label);

  void incr();
  void decr();
  void click() {}
  void draw(const Rectf& rect, bool is_active);
  boost::signal<void (int)>& sig_change() { return on_change; }
};

/** A slider widget for use in volume controls, gamma controls and
    things like that */
class SliderMenuItem : public MenuItem 
{
public:
  int value;
  int min_value;
  int max_value;
  int step;
  boost::signal<void (int)> on_change;
public:  
  SliderMenuItem(MenuComponent* parent_, 
                 const std::string& label_, int value_, int mix_value_ = 0, int max_value_ = 100, int step = 10);
  void incr();
  void decr();
  void click() {}
  void draw(const Rectf& rect, bool is_active);
  boost::signal<void (int)>& sig_change() { return on_change; }
};

class ButtonMenuItem : public MenuItem 
{
public:
  boost::signal<void ()> on_click;

public:  
  ButtonMenuItem(MenuComponent* parent_, const std::string& label_);
  void incr() {}
  void decr() {}
  void click();
  void draw(const Rectf& rect, bool is_active);
  boost::signal<void ()>& sig_click() { return on_click; }
};

} // namespace gui

#endif

/* EOF */
