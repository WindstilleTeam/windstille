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

#include "properties/property_set.hpp"

PropertySet::PropertySet(const std::string& name_, PropertySet* parent_)
  : parent(parent_),
    name(name_),
    properties()
{  
}

PropertySet::~PropertySet()
{
  for(std::map<std::string, Property*>::iterator i = properties.begin(); i != properties.end(); ++i)
    {
      delete i->second;
    }
}
  
void
PropertySet::add(const std::string& name_, Property* property)
{
  Property*& i = properties[name_];
  if (i)
    {
      std::ostringstream str;
      str << "PropertySet::add: Name conflict: '" << name_ << "' already used";
      throw std::runtime_error(str.str());
    }
  else
    {
      i = property;
    }
}

Property*
PropertySet::get(const std::string& name_) const
{
  std::map<std::string, Property*>::const_iterator i = properties.find(name_);
  if (i == properties.end())
    {
      if (parent)
        return parent->get(name_);
      else
        return 0;
    }
  else
    {
      return i->second;
    }
}

void
PropertySet::get_properties(std::vector<std::string>& lst) const
{
  if (parent)
    parent->get_properties(lst);
  
  for(std::map<std::string, Property*>::const_iterator i = properties.begin(); i != properties.end(); ++i)
    {
      lst.push_back(i->first);
    }
}

/* EOF */
