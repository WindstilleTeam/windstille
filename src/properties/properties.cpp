/*
**  Windstille - A Sci-Fi Action-Adventure Game
**  Copyright (C) 2009 Ingo Ruhnke <grumbel@gmx.de>
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

#include <sstream>
#include <stdexcept>

#include "property.hpp"
#include "property_set.hpp"
#include "properties.hpp"

Properties::Properties(PropertySet* pset_, void* object_)
  : pset(pset_),
    object(object_)
{
}

Property& 
Properties::get(const std::string& name) const
{
  Property* prop = pset->get(name);
  if (!prop)
    {
      std::ostringstream str;
      str << "Properties: " << pset->get_name() << ": property " << name << " not found";
      throw std::runtime_error(str.str());
    }
  else
    {
      return *prop;
    }
}

int
Properties::get_int(const std::string& name)  const
{
  return get(name).get_int(object);
}

float
Properties::get_float(const std::string& name) const
{
  return get(name).get_float(object);
}

std::vector<std::string>
Properties::get_properties() const
{
  std::vector<std::string> lst;
  pset->get_properties(lst);
  return lst;
}

/* EOF */
