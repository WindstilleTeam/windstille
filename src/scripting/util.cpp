/*
**  Windstille - A Sci-Fi Action-Adventure Game
**  Copyright (C) 2007 Matthias Braun <matze@braunis.de>,
**                     Ingo Ruhnke <grumbel@gmail.com>
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

#include <fstream>
#include <iostream>
#include <memory>
#include <sstream>
#include <stdio.h>

#include <sexp/parser.hpp>

#include "util/file_writer.hpp"
#include "util/pathname.hpp"

#include "scripting/util.hpp"

namespace Scripting {

std::string sq_to_lisp_string(std::string sq_str)
{
  for (unsigned i = 0; i != sq_str.size(); ++i)
  {
    if (sq_str[i] == '_')
      sq_str[i] = '-';
  }

  return sq_str;
}

void sq_to_lisp(HSQUIRRELVM v, std::vector<sexp::Value>& entries)
{
  switch(sq_gettype(v, -1))
  {
    case OT_INTEGER: {
      SQInteger val;
      sq_getinteger(v, -1, &val);
      entries.push_back(sexp::Value::integer(static_cast<int>(val)));
      break;
    }
    case OT_FLOAT: {
      float val;
      sq_getfloat(v, -1, &val);
      entries.push_back(sexp::Value::real(val));
      break;
    }
    case OT_STRING: {
      const char* str;
      sq_getstring(v, -1, &str);
      entries.push_back(sexp::Value::string(str));
      break;
    }
    case OT_BOOL: {
      SQBool boolean;
      sq_getbool(v, -1, &boolean);
      entries.push_back(sexp::Value::boolean(static_cast<bool>(boolean)));
      break;
    }
    case OT_ARRAY:
    case OT_TABLE: {
      table_to_lisp(v, -1, entries);
      break;
    }
    default:
      std::cerr << "Unsupported value type in table\n";
      break;
  }
}

void table_to_lisp(HSQUIRRELVM v, int idx, std::vector<sexp::Value>& entries)
{
  SQObjectType type = sq_gettype(v, idx);

  if (type != OT_TABLE && type != OT_ARRAY)
  {
    print_squirrel_stack(v, "table_to_lisp");
    throw std::runtime_error("table_to_lisp: no table or array at the given index");
  }
  else
  {
    sq_push(v, idx);

    // iterate table
    sq_pushnull(v);
    while (SQ_SUCCEEDED(sq_next(v, -2)))
    {
      if (type == OT_ARRAY)
      {
        sq_to_lisp(v, entries);
      }
      else if (type == OT_TABLE)
      {
        // table/array is -3, key is -2, value -1 now
        if (sq_gettype(v, -2) != OT_STRING)
        {
          std::cerr << "Table contains a non string key\n";
        }
        else
        {
          const char* key = nullptr;
          sq_getstring(v, -2, &key);
          std::string lisp_key = sq_to_lisp_string(key);

          std::vector<sexp::Value> childs;
          childs.push_back(sexp::Value::symbol(lisp_key));
          sq_to_lisp(v, childs);
          entries.push_back(sexp::Value::array(std::move(childs)));
        }
      }

      // pop key and value
      sq_pop(v, 2);
    }

    // pop iterator and table
    sq_pop(v, 2);
  }
}

std::string squirrel2string(HSQUIRRELVM v, int i)
{
  std::ostringstream os;
  switch(sq_gettype(v, i))
  {
    case OT_NULL:
      os << "<null>";
      break;
    case OT_BOOL: {
      SQBool p;
      sq_getbool(v, i, &p);
      if (p)
        os << "true";
      else
        os << "false";
      break;
    }
    case OT_INTEGER: {
      SQInteger val;
      sq_getinteger(v, i, &val);
      os << val;
      break;
    }
    case OT_FLOAT: {
      float val;
      sq_getfloat(v, i, &val);
      os << val;
      break;
    }
    case OT_STRING: {
      const char* val;
      sq_getstring(v, i, &val);
      os << "\"" << val << "\"";
      break;
    }
    case OT_TABLE: {
      bool first = true;
      os << "{";
      sq_pushnull(v);  //null iterator
      while(SQ_SUCCEEDED(sq_next(v,i-1)))
      {
        if (!first)
        {
          os << ", \n";
        }
        first = false;

        //here -1 is the value and -2 is the key
        os << squirrel2string(v, -2) << " => "
           << squirrel2string(v, -1);

        sq_pop(v,2); //pops key and val before the nex iteration
      }
      sq_pop(v, 1);
      os << "}";
      break;
    }
    case OT_ARRAY: {
      bool first = true;
      os << "[";
      sq_pushnull(v);  //null iterator
      while(SQ_SUCCEEDED(sq_next(v,i-1)))
      {
        if (!first)
        {
          os << ", ";
        }
        first = false;

        //here -1 is the value and -2 is the key
        // we ignore the key, since that is just the index in an array
        os << squirrel2string(v, -1);

        sq_pop(v,2); //pops key and val before the nex iteration
      }
      sq_pop(v, 1);
      os << "]";
      break;
    }
    case OT_USERDATA:
      os << "<userdata>";
      break;
    case OT_CLOSURE:
      os << "<closure (function)>";
      break;
    case OT_NATIVECLOSURE:
      os << "<native closure (C function)>";
      break;
    case OT_GENERATOR:
      os << "<generator>";
      break;
    case OT_USERPOINTER:
      os << "userpointer";
      break;
    case OT_THREAD:
      os << "<thread>";
      break;
    case OT_CLASS:
      os << "<class>";
      break;
    case OT_INSTANCE:
      os << "<instance>";
      break;
    default:
      os << "<unknown>";
      break;
  }
  return os.str();
}

void print_squirrel_stack(HSQUIRRELVM v, const std::string& context)
{
  if (context.empty())
    printf(",-------------------------------------------------------------\n");
  else
    printf(",-------------[ %s ]---------------------------------------\n", context.c_str());

  SQInteger count = sq_gettop(v);
  for(int i = 1; i <= count; ++i)
  {
    printf("| %d: ",i);
    switch(sq_gettype(v, i))
    {
      case OT_NULL:
        printf("null");
        break;
      case OT_INTEGER: {
        SQInteger val;
        sq_getinteger(v, i, &val);
        printf("integer (%d)", static_cast<int>(val));
        break;
      }
      case OT_FLOAT: {
        float val;
        sq_getfloat(v, i, &val);
        printf("float (%f)", static_cast<double>(val));
        break;
      }
      case OT_STRING: {
        const char* val;
        sq_getstring(v, i, &val);
        printf("string (%s)", val);
        break;
      }
      case OT_TABLE:
        printf("table");
        break;
      case OT_ARRAY:
        printf("array");
        break;
      case OT_USERDATA:
        printf("userdata");
        break;
      case OT_CLOSURE:
        printf("closure(function)");
        break;
      case OT_NATIVECLOSURE:
        printf("native closure(C function)");
        break;
      case OT_GENERATOR:
        printf("generator");
        break;
      case OT_USERPOINTER:
        printf("userpointer");
        break;
      case OT_THREAD:
        printf("thread");
        break;
      case OT_CLASS:
        printf("class");
        break;
      case OT_INSTANCE:
        printf("instance");
        break;
      default:
        printf("unknown?!?");
        break;
    }
    printf("\n");
  }
  printf("'-------------------------------------------------------------\n");
}

} // namespace Scripting

/* EOF */
