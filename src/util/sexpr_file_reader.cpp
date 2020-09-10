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

#include <string.h>
#include <stdint.h>

#include "lisp/getters.hpp"
#include "util/file_reader_impl.hpp"
#include "util/sexpr_file_reader.hpp"

using namespace lisp;

class SExprFileReaderImpl: public FileReaderImpl
{
public:
  sexp::Value sexpr;

public:
  SExprFileReaderImpl(sexp::Value const& sexpr_) :
    sexpr(sexpr_)
  {
    assert(sexpr.is_array() &&
           sexpr.as_array().size() >= 1);
  }

  ~SExprFileReaderImpl() override
  {
  }

  std::string get_name() const override
  {
    return sexpr.as_array()[0].as_string();
  }

  bool read_int(const char* name, int& v) const override
  {
    sexp::Value const* item = get_subsection_item(name);
    if (item && item->is_integer())
    {
      v = item->as_int();
      return true;
    }
    return false;
  }

  bool read_uint32(const char* name, uint32_t& v) const
  {
    sexp::Value const* item = get_subsection_item(name);
    if (item && item->is_integer())
    {
      // FIXME: Not good: overflow
      v = static_cast<uint32_t>(item->as_int());
      return true;
    }
    return false;
  }

  bool read_float(const char* name, float& v) const override
  {
    sexp::Value const* item = get_subsection_item(name);
    if (item)
    {
      if (item->is_real())
      {
        v = item->as_float();
        return true;
      }
      else if (item->is_integer())
      {
        v = static_cast<float>(item->as_int());
        return true;
      }
      else
      {
        return false;
      }
    }
    return false;
  }

  bool read_bool  (const char* name, bool& v) const override
  {
    sexp::Value const* item = get_subsection_item(name);
    if (item && item->is_boolean())
    {
      v = item->as_bool();
      return true;
    }
    return false;
  }

  bool read_string(const char* name, std::string& v) const override
  {
    sexp::Value const* sub = get_subsection(name);
    if (sub)
    {
      v = "";
      for(size_t i = 1; i < sub->as_array().size(); ++i)
      {
        sexp::Value const& item = sub->as_array()[i];
        if (item.is_string())
        {
          v += item.as_string();
        }
        else if (item.is_symbol())
        {
          v += item.as_string();
        }
      }
      return true;
    }
    return false;
  }

  bool get(const char* name, std::vector<int>&   v) const override
  {
    sexp::Value const* sub = get_subsection(name);
    if (sub)
      return property_get(*sub, v);
    else
      return false;
  }

  bool get(const char* name, std::vector<bool>&   v) const override
  {
    sexp::Value const* sub = get_subsection(name);
    if (sub)
      return property_get(*sub, v);
    else
      return false;
  }

  bool get(const char* name, std::vector<std::string>&   v) const override
  {
    sexp::Value const* sub = get_subsection(name);
    if (sub)
      return property_get(*sub, v);
    else
      return false;
  }

  bool get(const char* name, std::vector<float>& v) const override
  {
    sexp::Value const* sub = get_subsection(name);
    if (sub)
      return property_get(*sub, v);
    else
      return false;
  }

  bool read_section(const char* name, FileReader& v) const override
  {
    sexp::Value const* cur = get_subsection(name);
    if (cur)
    {
      v = SExprFileReader(*cur);
      return true;
    }
    return false;
  }

  std::vector<FileReader> get_sections() const override
  {
    std::vector<FileReader> lst;
    for(size_t i = 1; i < sexpr.as_array().size(); ++i)
    { // iterate over subsections
      lst.push_back(SExprFileReader(sexpr.as_array()[i]));
    }
    return lst;
  }

  std::vector<std::string> get_section_names() const override
  {
    std::vector<std::string> lst;

    for(size_t i = 1; i < sexpr.as_array().size(); ++i)
    { // iterate over subsections
      sexp::Value const& sub = sexpr.as_array()[i];
      lst.push_back(sub.as_array()[0].as_string());
    }

    return lst;
  }

private:
  sexp::Value const* get_subsection_item(const char* name) const
  {
    sexp::Value const* sub = get_subsection(name);
    if (sub && sub->as_array().size() == 2)
    {
      return &(sub->as_array()[1]);
    }
    return nullptr;
  }

  sexp::Value const* get_subsection(const char* name) const
  {
    for(size_t i = 1; i < sexpr.as_array().size(); ++i)
    { // iterate over subsections
      sexp::Value const& sub = sexpr.as_array()[i];
      if (sub.as_array()[0].as_string() == name)
        return &sub;
    }
    return nullptr;
  }

private:
  SExprFileReaderImpl(const SExprFileReaderImpl&);
  SExprFileReaderImpl& operator=(const SExprFileReaderImpl&);
};

SExprFileReader::SExprFileReader(sexp::Value const& sexpr)
  : FileReader(std::shared_ptr<FileReaderImpl>(new SExprFileReaderImpl(sexpr)))
{
}

/* EOF */
