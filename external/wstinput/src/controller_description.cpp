// Windstille Input Library
// Copyright (C) 2005-2020 Ingo Ruhnke <grumbel@gmail.com>
//
// This program is free software: you can redistribute it and/or modify it
// under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation, either version 3 of the License, or (at your
// option) any later version.
//
// This program is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
// or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public
// License for more details.
//
// You should have received a copy of the GNU Lesser General Public License
// along with this program. If not, see <http://www.gnu.org/licenses/>.

#include <stdexcept>

#include "controller_description.hpp"

namespace wstinput {

ControllerDescription::ControllerDescription()
  : str_to_event(),
    id_to_event()
{
}

ControllerDescription::~ControllerDescription()
{
}

void
ControllerDescription::add_button(const std::string& name, int id)
{
  InputEventDefinition event;

  event.type = BUTTON_EVENT;
  event.name = name;
  event.id   = id;

  str_to_event[event.name] = event;
  id_to_event[event.id]    = event;
}

void
ControllerDescription::add_pointer(const std::string& name, int id)
{
  InputEventDefinition event;

  event.type = POINTER_EVENT;
  event.name = name;
  event.id   = id;

  str_to_event[event.name] = event;
  id_to_event[event.id]    = event;
}

void
ControllerDescription::add_ball(const std::string& name, int id)
{
  InputEventDefinition event;

  event.type = BALL_EVENT;
  event.name = name;
  event.id   = id;

  str_to_event[event.name] = event;
  id_to_event[event.id]    = event;
}

void
ControllerDescription::add_axis(const std::string& name, int id)
{
  InputEventDefinition event;

  event.type = AXIS_EVENT;
  event.name = name;
  event.id   = id;

  str_to_event[event.name] = event;
  id_to_event[event.id]    = event;
}

const InputEventDefinition&
ControllerDescription::get_definition(int id) const
{
  std::map<int, InputEventDefinition>::const_iterator i = id_to_event.find(id);
  if (i == id_to_event.end())
    throw std::runtime_error("Unknown event id");

  return i->second;
}

const InputEventDefinition&
ControllerDescription::get_definition(const std::string& name) const
{
  std::map<std::string, InputEventDefinition>::const_iterator i = str_to_event.find(name);
  if (i == str_to_event.end())
    throw std::runtime_error("Unknown event str: " + name);

  return i->second;
}

int
ControllerDescription::get_max_id() const
{
  int result = 0;
  for(auto const& item : id_to_event) {
    result = std::max(result, item.first);
  }
  return result;
}

} // namespace wstinput

/* EOF */
