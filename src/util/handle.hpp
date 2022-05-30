/*
**  Windstille - A Sci-Fi Action-Adventure Game
**  Copyright (C) 2007 Ingo Ruhnke <grumbel@gmail.com>
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

#ifndef HEADER_WINDSTILLE_UTIL_HANDLE_HPP
#define HEADER_WINDSTILLE_UTIL_HANDLE_HPP

#include <vector>

namespace windstille {

template<typename Data> class HandleManager;

template<typename Data>
class Handle
{
private:
  HandleManager<Data>* manager;

  unsigned int index;
  unsigned int magic;

public:
  Handle(HandleManager<Data>* manager_,
         unsigned int index_,
         unsigned int magic_)
    : manager(manager_),
      index(index_),
      magic(magic_)
  {
  }

  bool is_null() {
    return manager->dereference(index, magic) != 0;
  }

  Data* operator->() {
    return manager->dereference(index, magic);
  }

  Data& operator*() {
    return manager->dereference(index, magic);
  }
};

template<typename Data>
class HandleManager
{
private:
  std::vector<Data>        data_lst;
  std::vector<unsigned int> magic_lst;

  /** List of indexes in \a data_lst and \a magic_lst that aren't
      used */
  std::vector<unsigned int>  free_list;

  unsigned int next_magic;

public:
  HandleManager()
    : next_magic(0)
  {
  }

  Handle<Data> aquire(Data const& data)
  {
    if (free_list.empty())
    {
      data_lst.push_back(data);
      magic_lst.push_back(next_magic);

      return Handle<Data>(this, next_magic++);
    }
    else
    {
      unsigned int index = free_list.back();
      free_list.pop_back();

      data_lst[index]  = data;
      magic_lst[index] = next_magic;

      return Handle<Data>(this, next_magic++);
    }
  }

  void release(Handle<Data> const& handle)
  {
    if (valid(handle))
    {
      delete data_lst[handle.index];
      data_lst[handle.index] = 0;

      free_list.push_back(handle.index);
    }
    else
    {
      // invalid handle
    }
  }

  bool valid(Handle<Data> const& handle)
  {
    return (handle.index < data_lst.size() &&
            magic_lst[handle.index] == handle.magic);
  }

  Data& dereference(Handle<Data> const& handle)
  {
    if (valid)
    {
      return data_lst[handle.index];
    }
    else
    {
      // invalid handle
    }
  }
};

} // namespace windstille

#endif

/* EOF */
