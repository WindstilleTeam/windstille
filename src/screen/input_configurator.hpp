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

#ifndef HEADER_WINDSTILLE_INPUT_INPUT_CONFIGURATOR_HPP
#define HEADER_WINDSTILLE_INPUT_INPUT_CONFIGURATOR_HPP

#include <sstream>
#include <vector>

#include <wstdisplay/font/text_area.hpp>

#include "screen/screen.hpp"

struct ConfigureItem
{
  enum Mode { CONFIGURE_AXIS, CONFIGURE_BUTTON };
  Mode mode;
  int  event_id;
};

/** */
class InputConfigurator : public Screen
{
private:
  std::vector<ConfigureItem> items;
  bool wait_for_plus;
  SDL_Event minus;
  std::ostringstream out;
  wstdisplay::TextArea area;

public:
  InputConfigurator();
  ~InputConfigurator() override;

  void draw(wstdisplay::GraphicsContext& gc) override;
  void update(float delta, const Controller& controller) override;
  void handle_event(const SDL_Event& event) override;
  void add_configure_item(ConfigureItem::Mode mode, int event_id);
  void next_item();
  void print_item();

private:
  InputConfigurator(const InputConfigurator&);
  InputConfigurator& operator=(const InputConfigurator&);
};

#endif

/* EOF */
