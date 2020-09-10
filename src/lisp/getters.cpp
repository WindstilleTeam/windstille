#include "lisp/getters.hpp"

namespace lisp
{

bool get(sexp::Value const& lisp, bool& val)
{
  if (!lisp.is_boolean())
    return false;

  val = lisp.as_bool();
  return true;
}

bool get(sexp::Value const& lisp, float& val)
{
  if (lisp.is_integer()) {
    val = static_cast<float>(lisp.as_int());
    return true;
  } else if (lisp.is_real()) {
    val = lisp.as_float();
    return true;
  } else {
    return false;
  }
}

bool get(sexp::Value const& lisp, int& val)
{
  if (!lisp.is_integer())
    return false;

  val = lisp.as_int();
  return true;
}

bool get(sexp::Value const& lisp, std::string& val)
{
  if (!lisp.is_string())
    return false;

  val = lisp.as_string();
  return true;
}

}
