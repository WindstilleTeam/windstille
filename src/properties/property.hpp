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

#ifndef HEADER_WINDSTILLE_PROPERTIES_PROPERTY_HPP
#define HEADER_WINDSTILLE_PROPERTIES_PROPERTY_HPP


class Property
{
protected:
  enum Type { INT_TYPE, FLOAT_TYPE, BOOL_TYPE } type;

public:
  Property(Type type_)
    : type(type_)
  {}
  virtual ~Property() {}

  virtual bool&  get_bool(void* obj)  const = 0;
  virtual int&   get_int(void* obj)   const = 0;
  virtual float& get_float(void* obj) const = 0;

private:
  Property(const Property&);
  Property& operator=(const Property&);
};

template<class C>
class PropertyImpl : public Property
{
private:
  union {
    bool  C::*bool_value;
    int   C::*int_value;
    float C::*float_value;
  } value;

  PropertyImpl(Type type_)
    : Property(type_),
      value()
  {}

public:
  static PropertyImpl<C>* create_int(int C::*int_value)
  {
    PropertyImpl<C>* prop = new PropertyImpl<C>(INT_TYPE);
    prop->value.int_value = int_value;
    return prop;
  }

  static PropertyImpl<C>* create_bool(bool C::*bool_value)
  {
    PropertyImpl<C>* prop = new PropertyImpl<C>(BOOL_TYPE);
    prop->value.bool_value = bool_value;
    return prop;
  }

  virtual bool& get_bool(void* obj) const
  {
    return static_cast<C*>(obj)->*(value.bool_value);
  }

  virtual int& get_int(void* obj) const
  {
    return static_cast<C*>(obj)->*(value.int_value);
  }

  virtual float& get_float(void* obj) const
  {
    return static_cast<C*>(obj)->*(value.float_value);
  }

private:
  PropertyImpl(const PropertyImpl&);
  PropertyImpl& operator=(const PropertyImpl&);
};

#endif

/* EOF */
