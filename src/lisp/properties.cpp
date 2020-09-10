#include <iostream>

#include "lisp/properties.hpp"

namespace lisp
{

Properties::Properties(sexp::Value const& lisp) :
  properties()
{
  if (lisp)
  {
    if (!lisp.is_array())
      throw std::runtime_error("Lisp is not a list");

    for (size_t i = 0; i < lisp.as_array().size(); ++i)
    {
      sexp::Value const& child = lisp.as_array()[i];
      if (i == 0 && child.is_symbol())
        continue;

      if (!child.is_array())
        throw std::runtime_error("child of properties lisp is not a list");

      if (child.as_array().size() > 1)
      {
        sexp::Value const& name = child.as_array()[0];
        if (!name.is_symbol())
          throw std::runtime_error("property has no symbol as name");

        properties.insert(std::make_pair(std::string(name.as_string()), ListEntry(child)));
      }
    }
  }
}

Properties::~Properties()
{
}

void
Properties::print_unused_warnings(const std::string& context) const
{
  for(PropertyMap::const_iterator i = properties.begin();
      i != properties.end(); ++i) {
    if(i->second.used)
      continue;

    std::cout << "Warning: property '" << i->first << "' not used (in "
              << context << ")\n";
  }
}

}
