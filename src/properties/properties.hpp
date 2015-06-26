/*
**  Windstille - A Sci-Fi Action-Adventure Game
**  Copyright (C) 2009 Ingo Ruhnke <grumbel@gmail.com>
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

#ifndef HEADER_WINDSTILLE_PROPERTIES_PROPERTIES_HPP
#define HEADER_WINDSTILLE_PROPERTIES_PROPERTIES_HPP


class PropertySet;
class Property;

/** Simple Facade class around PropertySet and Property. Properties is
    used to provide introspection for C++ classes */
class Properties
{
private:
  PropertySet* pset;
  void*        object;

public:
  Properties(PropertySet* pset_, void* object_);

  Property& get(const std::string& name) const;
  int   get_int(const std::string& name) const;
  float get_float(const std::string& name) const;

  std::vector<std::string> get_properties() const;
};

#endif

/* EOF */
