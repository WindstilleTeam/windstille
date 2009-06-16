/*
**  Windstille - A Sci-Fi Action-Adventure Game
**  Copyright (C) 2005 Ingo Ruhnke <grumbel@gmx.de>
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

#include <assert.h>
#include <string.h>
#include <stdint.h>

#include "lisp/parser.hpp"
#include "lisp/getters.hpp"
#include "display/color.hpp"
#include "math/size.hpp"
#include "math/vector2f.hpp"
#include "math/vector3.hpp"
#include "file_reader_impl.hpp"
#include "sexpr_file_reader.hpp"

class SExprFileReaderImpl: public FileReaderImpl
{
public:
  /** Pointer to the top-most lisp element, only here for purpose of
      being able to delete it */
  const lisp::Lisp* root;
  const lisp::Lisp* sexpr;
  bool  delete_sexpr;

  SExprFileReaderImpl(const lisp::Lisp* root_, const lisp::Lisp* sexpr_)
    : root(root_),
      sexpr(sexpr_), 
      delete_sexpr(false)
  {
    assert(sexpr && 
           sexpr->get_type() == lisp::Lisp::TYPE_LIST &&
           sexpr->get_list_size() >= 1);
  }

  SExprFileReaderImpl(const lisp::Lisp* sexpr_, bool delete_sexpr_) 
    : root(0),
      sexpr(sexpr_), 
      delete_sexpr(delete_sexpr_)
  {
    assert(sexpr && 
           sexpr->get_type() == lisp::Lisp::TYPE_LIST &&
           sexpr->get_list_size() >= 1);
  }

  ~SExprFileReaderImpl()
  {
    if (root)
      delete root;

    if (delete_sexpr)
      delete sexpr;
  }

  std::string get_name() const 
  {
    return sexpr->get_list_elem(0)->get_symbol();
  }

  bool read_int(const char* name, int& v) const 
  {
    lisp::Lisp* item = get_subsection_item(name);
    if (item && item->get_type() == lisp::Lisp::TYPE_INT)
      {
        v = item->get_int();
        return true;
      }
    return false;
  }

  bool read_uint32(const char* name, uint32_t& v) const 
  {
    lisp::Lisp* item = get_subsection_item(name);
    if (item && item->get_type() == lisp::Lisp::TYPE_INT)
      {
        // FIXME: Not good: overflow
        v = item->get_int();
        return true;
      }
    return false;
  }

  bool read_float(const char* name, float& v) const 
  {
    lisp::Lisp* item = get_subsection_item(name);
    if (item)
      {
        if (item->get_type() == lisp::Lisp::TYPE_FLOAT)
          {
            v = item->get_float();
            return true;
          }
        else if (item->get_type() == lisp::Lisp::TYPE_INT)
          {
            v = item->get_int();
            return true;
          }
        else
          {
            return false;
          }
      }
    return false;
  }

  bool read_bool  (const char* name, bool& v) const 
  {
    lisp::Lisp* item = get_subsection_item(name);
    if (item && item->get_type() == lisp::Lisp::TYPE_BOOL)
      {
        v = item->get_bool();
        return true;
      }
    return false;
  }

  bool read_string(const char* name, std::string& v) const 
  {
    lisp::Lisp* sub = get_subsection(name);
    if (sub)
      {
        v = "";
        for(size_t i = 1; i < sub->get_list_size(); ++i)
          {
            lisp::Lisp* item = sub->get_list_elem(i);
            if (item->get_type() == lisp::Lisp::TYPE_STRING)
              {
                v += item->get_string();
              }
            else if (item->get_type() == lisp::Lisp::TYPE_SYMBOL)
              {
                v += item->get_symbol();
              }
          }
        return true;
      }
    return false;
  }

  bool get(const char* name, std::vector<int>&   v) const
  {
    lisp::Lisp* sub = get_subsection(name);
    if (sub)
      return property_get(sub, v);
    else 
      return false;
  }

  bool get(const char* name, std::vector<bool>&   v) const
  {
    lisp::Lisp* sub = get_subsection(name);
    if (sub)
      return property_get(sub, v);
    else 
      return false;
  }

  bool get(const char* name, std::vector<std::string>&   v) const
  {
    lisp::Lisp* sub = get_subsection(name);
    if (sub)
      return property_get(sub, v);
    else 
      return false;
  }

  bool get(const char* name, std::vector<float>& v) const
  {
    lisp::Lisp* sub = get_subsection(name);
    if (sub)
      return property_get(sub, v);
    else 
      return false;
  }

  bool read_section(const char* name, FileReader& v) const 
  {
    lisp::Lisp* cur = get_subsection(name);
    if (cur)
      {
        v = SExprFileReader(cur);
        return true;
      }
    return false;
  }

  std::vector<FileReader> get_sections() const 
  {
    std::vector<FileReader> lst;
    for(size_t i = 1; i < sexpr->get_list_size(); ++i)
      { // iterate over subsections
        lst.push_back(SExprFileReader(sexpr->get_list_elem(i)));
      }
    return lst;
  }

  std::vector<std::string> get_section_names() const 
  {
    std::vector<std::string> lst;

    for(size_t i = 1; i < sexpr->get_list_size(); ++i)
      { // iterate over subsections
        lisp::Lisp* sub = sexpr->get_list_elem(i);
        lst.push_back(sub->get_list_elem(0)->get_symbol());
      }

    return lst;
  }

private:
  lisp::Lisp* get_subsection_item(const char* name) const
  {
    lisp::Lisp* sub = get_subsection(name);
    if (sub && sub->get_list_size() == 2)
      {
        return sub->get_list_elem(1);
      }
    return 0;
  }

  lisp::Lisp* get_subsection(const char* name) const
  {
    for(size_t i = 1; i < sexpr->get_list_size(); ++i)
      { // iterate over subsections
        lisp::Lisp* sub = sexpr->get_list_elem(i);
        if (strcmp(sub->get_list_elem(0)->get_symbol(), name) == 0)
          return sub;
      }
    return 0;
  } 

};

SExprFileReader::SExprFileReader(const lisp::Lisp* sexpr, bool delete_sexpr)
  : FileReader(boost::shared_ptr<FileReaderImpl>(new SExprFileReaderImpl(sexpr, delete_sexpr)))
{
}

SExprFileReader::SExprFileReader(const lisp::Lisp* root, const lisp::Lisp* sexpr)
  : FileReader(boost::shared_ptr<FileReaderImpl>(new SExprFileReaderImpl(root, sexpr)))
{
}

/* EOF */
