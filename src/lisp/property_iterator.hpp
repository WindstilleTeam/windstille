#ifndef __PROPERTY_ITERATOR_HPP__
#define __PROPERTY_ITERATOR_HPP__

#include <map>
#include <string>

namespace lisp
{

struct ListEntry {
  ListEntry(const lisp::Lisp* lisp_)
    : lisp(lisp_), used(false)
  {}

  const Lisp* lisp;
  bool used;
};
typedef std::multimap<std::string, ListEntry> PropertyMap;

template<typename T>
class PropertyIterator
{
public:
  PropertyIterator()
  {
    end = i;
  }

  T* operator ->() const
  {
    return &currentval;
  }

  T operator*() const
  {
    return currentval;
  }

  bool next() {
    bool res;
    do {
      if(i == end)
        return false;
      res = property_get(i->second.lisp, currentval);
      if(res) {
        i->second.used = true;
        current_item = i->first;
      }
      ++i;
      if(res)
        return true;
    } while(true);
  }

  const std::string& item() const
  {
    return current_item;
  }

private:
  friend class Properties;

  PropertyIterator(PropertyMap::iterator begin_, PropertyMap::iterator end_)
    : i(begin_), end(end_)
  {
  }

  PropertyMap::iterator i;
  PropertyMap::iterator end;
  std::string current_item;
  T currentval;
};

}

#endif
