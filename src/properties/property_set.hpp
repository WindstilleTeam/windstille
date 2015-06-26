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

#ifndef HEADER_WINDSTILLE_PROPERTIES_PROPERTY_SET_HPP
#define HEADER_WINDSTILLE_PROPERTIES_PROPERTY_SET_HPP

#include <vector>
#include <map>

#include "properties/property.hpp"

class PropertySet
{
private:
  PropertySet* parent;
  std::string  name;
  std::map<std::string, Property*> properties;

public:
  PropertySet(const std::string& name, PropertySet* parent = 0);
  ~PropertySet();

  std::string get_name() const { return name; }

  void add(const std::string& name, Property* property);

  template<class C>
  void add_int(const std::string& name_, bool C::*ptr)
  {
    add(name_, PropertyImpl<C>::create_int(ptr));
  }

  template<class C>
  void add_bool(const std::string& name_, bool C::*ptr)
  {
    add(name_, PropertyImpl<C>::create_bool(ptr));
  }

  Property* get(const std::string& name_) const;

  void get_properties(std::vector<std::string>& lst) const;

private:
  PropertySet(const PropertySet&);
  PropertySet& operator=(const PropertySet&);
};

#endif

/* EOF */
