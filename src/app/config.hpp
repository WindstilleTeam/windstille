/*
**  Windstille - A Sci-Fi Action-Adventure Game
**  Copyright (C) 2000,2005 Ingo Ruhnke <grumbel@gmail.com>
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

#ifndef HEADER_WINDSTILLE_APP_CONFIG_HPP
#define HEADER_WINDSTILLE_APP_CONFIG_HPP

#include <map>
#include <stdexcept>
#include <sstream>

#include "util/file_writer.hpp"

extern std::vector<std::string> arg_files;

class ConfigValueBase
{
private:
  std::string name;
  std::string docstring;

protected:
  bool has_been_set;
  bool do_save;

public:
  ConfigValueBase(std::string const& name_, std::string const& docstring_, bool do_save_)
    : name(name_), docstring(docstring_),
      has_been_set(false),
      do_save(do_save_)
  {
  }

  virtual ~ConfigValueBase() {}

  std::string const& get_name() { return name; }
  std::string const& get_docstring() { return docstring; }

  bool is_set() const {
    return has_been_set;
  }

  bool should_be_saved() const {
    return do_save;
  }

  virtual std::ostream& print(std::ostream& os) const =0;
  std::string str() const {
    std::ostringstream os;
    print(os);
    return os.str();
  }

  virtual void write(FileWriter& writer) =0;
};

template<class T>
class ConfigValue : public ConfigValueBase
{
private:
  T    data;

public:
  ConfigValue(std::string const& name_, std::string const& docstring_, bool do_save_, T const& data_) :
    ConfigValueBase(name_, docstring_, do_save_),
    data(data_)
  {
    has_been_set = true;
  }

  ConfigValue(std::string const& name_, std::string const& docstring_, bool do_save_) :
    ConfigValueBase(name_, docstring_, do_save_),
    data()
  {
    has_been_set = false;
  }

  T const& get() const {
    return data;
  }

  T& get() {
    return data;
  }

  T& set(T const& data_) {
    data = data_;
    has_been_set  = true;
    return data;
  }

  ConfigValue<T>& operator=(T const& data_) {
    set(data_);
    return *this;
  }

  void write(FileWriter& writer) override;

  std::ostream& print(std::ostream& os) const override {
    return (os << data);
  }
};

inline std::ostream& operator<<(std::ostream &o, ConfigValueBase const& value)
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

  bool has_key(std::string const& name);

  bool is_set(std::string const& name);

  template<class T>
  ConfigValue<T>& get(std::string const& name) const {
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

  std::string get_string(std::string const& name) const;
  bool        get_bool  (std::string const& name) const;
  int         get_int   (std::string const& name) const;
  float       get_float (std::string const& name) const;

  void set_string(std::string const& name, std::string const& value);
  void set_bool  (std::string const& name, bool  value);
  void set_int   (std::string const& name, int   value);
  void set_float (std::string const& name, float value);

  void debug_print(std::ostream& out);
};

extern Config config;

#endif

/* EOF */
