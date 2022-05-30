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

namespace windstille {


class PropertySet
{
private:
  PropertySet* parent;
  std::string  name;
  std::map<std::string, Property*> properties;

public:
  PropertySet(std::string const& name, PropertySet* parent = nullptr);
  ~PropertySet();

  std::string get_name() const { return name; }

  void add(std::string const& name, Property* property);

  template<class C>
  void add_int(std::string const& name_, bool C::*ptr)
  {
    add(name_, PropertyImpl<C>::create_int(ptr));
  }

  template<class C>
  void add_bool(std::string const& name_, bool C::*ptr)
  {
    add(name_, PropertyImpl<C>::create_bool(ptr));
  }

  Property* get(std::string const& name_) const;

  void get_properties(std::vector<std::string>& lst) const;

private:
  PropertySet(PropertySet const&);
  PropertySet& operator=(PropertySet const&);
};


} // namespace windstille

#endif

/* EOF */
