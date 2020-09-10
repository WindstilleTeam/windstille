/** This header defines functions that assign the value of Lisp objects to
 * normal C++ datatypes.
 *
 * The get function is overloaded for a set of default datatypes. You can add
 * further overloads in your own code. The get functions return true if the
 * value could be converted and false otherwise.
 *
 * The property_get function expect a list whose values 1-n are transformed to a
 * C++ object. This is typically used from the code in the Properties class. You
 * can also write custom overloads in your code.
 */
#ifndef __GETTERS_HPP__
#define __GETTERS_HPP__

#include <assert.h>
#include <sexp/value.hpp>

namespace lisp {

bool get(sexp::Value const&, bool& val);
bool get(sexp::Value const&, float& val);
bool get(sexp::Value const&, int& val);
bool get(sexp::Value const&, std::string& val);
bool get(sexp::Value const&, sexp::Value& val);

template<typename T>
static inline bool property_get(sexp::Value const& sx, T& val)
{
  assert(sx.is_array());

  if (sx.as_array().size() != 2)
    return false;

  return get(sx.as_array()[1], val);
}

static inline bool property_get(sexp::Value const& sx, sexp::Value& val)
{
  val = sx;
  return true;
}

template<typename T>
static inline bool property_get(sexp::Value const& sx, std::vector<T>& list)
{
  assert(sx.is_array());

  list.clear();
  for(size_t n = 1; n < sx.as_array().size(); ++n) {
    T val;
    if(!get(sx.as_array()[n], val)) {
      list.clear();
      return false;
    }
    list.push_back(val);
  }
  return true;
}

} // namespace lisp

#endif

