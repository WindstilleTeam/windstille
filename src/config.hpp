/*  $Id$
**   __      __ __             ___        __   __ __   __
**  /  \    /  \__| ____    __| _/_______/  |_|__|  | |  |   ____
**  \   \/\/   /  |/    \  / __ |/  ___/\   __\  |  | |  | _/ __ \
**   \        /|  |   |  \/ /_/ |\___ \  |  | |  |  |_|  |_\  ___/
**    \__/\  / |__|___|  /\____ /____  > |__| |__|____/____/\___  >
**         \/          \/      \/    \/                         \/
**  Copyright (C) 2000,2005 Ingo Ruhnke <grumbel@gmx.de>
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

#ifndef HEADER_WINDSTILLE_CONFIG_HPP
#define HEADER_WINDSTILLE_CONFIG_HPP

#include <map>
#include <string>
#include <typeinfo>
#include <stdexcept>
#include "lisp/writer.hpp"

class ConfigValueBase
{
private:
  std::string name;
  std::string docstring;

protected:
  bool has_been_set;
  bool do_save;

public:
  ConfigValueBase(const std::string& name_, const std::string& docstring_, bool do_save_)
    : name(name_), docstring(docstring_),
      has_been_set(false),
      do_save(do_save_)
  {   
  }

  virtual ~ConfigValueBase() {}

  const std::string& get_name() { return name; }
  const std::string& get_docstring() { return docstring; }

  bool is_set() {
    return has_been_set;
  }

  bool should_be_saved() {
    return do_save;
  }

  virtual std::ostream& print(std::ostream& os) const =0;

  virtual void write(lisp::Writer& writer) =0;
};

template<class T>
class ConfigValue : public ConfigValueBase
{
private:
  T    data;

public:
  ConfigValue(const std::string& name_, const std::string& docstring_, bool do_save_, const T& data_)
    : ConfigValueBase(name_, docstring_, do_save_),
      data(data_)
  {
    has_been_set = true;
  }

  ConfigValue(const std::string& name_, const std::string& docstring_, bool do_save_)
    : ConfigValueBase(name_, docstring_, do_save_)
  {    
    has_been_set = false;
  }

  const T& get() const { 
    return data; 
  }

  T& get() { 
    return data;
  }

  T& set(const T& data_) {
    data = data_;
    has_been_set  = true;
    return data;
  }
  
  T& operator=(const T& data_) {
    return set(data_);
  }

  void write(lisp::Writer& writer);

  std::ostream& print(std::ostream& os) const {
    return (os << data);
  }
};

inline std::ostream& operator<<(std::ostream &o, const ConfigValueBase& value)
{
  return value.print(o);
}

class Config
{
private:
  typedef std::map<std::string, ConfigValueBase*> ConfigValues;
  ConfigValues config_values;

  void add(ConfigValueBase*);
public:
  Config();
  ~Config();

  void set_defaults();

  void parse_args(int argc, char** argv);

  void load();
  void save();

  bool has_key(const std::string& name);

  bool is_set(const std::string& name);

  template<class T>
  ConfigValue<T>& get(const std::string& name) const {
    ConfigValues::const_iterator i = config_values.find(name);
    if (i == config_values.end())
      {
        throw std::runtime_error("Error: unknown Config value: '" + name + "'");
      }
    else
      {
        ConfigValue<T>* ptr = dynamic_cast<ConfigValue<T>*>(i->second);
        if (ptr)
          return *ptr;
        else
          throw std::runtime_error("Error: Config value: '" + name + "' as wrong type");
      }
  }

  std::string get_string(const std::string& name) const;
  bool        get_bool  (const std::string& name) const;
  int         get_int   (const std::string& name) const;
  float       get_float (const std::string& name) const;

  void set_string(const std::string& name, const std::string& value);
  void set_bool  (const std::string& name, bool  value);
  void set_int   (const std::string& name, int   value);
  void set_float (const std::string& name, float value);

  void debug_print(std::ostream& out);
};

extern Config config;

#endif

/* EOF */
