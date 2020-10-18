/*
**  Windstille - A Sci-Fi Action-Adventure Game
**  Copyright (C) 2000 Ingo Ruhnke <grumbel@gmail.com>
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

#ifndef HEADER_WINDSTILLE_UTIL_FIELD_HPP
#define HEADER_WINDSTILLE_UTIL_FIELD_HPP

#include <vector>
#include <assert.h>

#include <geom/size.hpp>

template<class T>
class Field
{
private:
  int width;
  int height;
  std::vector<T> vec;

public:
  typedef typename std::vector<T>::iterator iterator;

  Field()
    : width(0),
      height(0),
      vec()
  {
  }

  Field (int w, int h)
    : width(w),
      height(h),
      vec(width * height)
  {
  }

  /** Creates a new field out of a subsection from an already excisting one
   *  @param arg_field The field thet gets copied
   *  @param w         width of the new field
   *  @param h         height of the new field
   *  @param pos_x     The position of the old field in the new resized one
   *  @param pos_y     The position of the old field in the new resized one */
  Field(const Field<T>& arg_field, int w, int h, int pos_x, int pos_y)
    : width (w), height (h), vec (width * height)
  {
    int start_x = std::max(0, -pos_x);
    int start_y = std::max(0, -pos_y);

    int end_x = std::min(arg_field.get_width(),  get_width()  - pos_x);
    int end_y = std::min(arg_field.get_height(), get_height() - pos_y);

    for(int y = start_y; y < end_y; ++y)
      for(int x = start_x; x < end_x; ++x)
        at(pos_x + x, pos_y + y) = arg_field.at(x, y);
  }

  const T& operator[] (int i) const {
    return vec[i];
  }

  T& operator[] (int i) {
    return vec[i];
  }

  T& operator() (int x, int y)
  {
    assert (x >= 0 || x < width || y >= 0 || y < height);
    return vec [width*y + x];
  }

  const T& operator() (int x, int y) const
  {
    assert (x >= 0 || x < width || y >= 0 || y < height);
    return vec [width*y + x];
  }

  inline const T& at (int x, int y) const {
    return (*this) (x, y);
  }

  inline T& at (int x, int y) {
    return (*this) (x, y);
  }

  /** Resize a field to a new size
   *  @param w     New width of the field
   *  @param h     New height of the field
   *  @param pos_x The position of the old field in the new resized one
   *  @param pos_y The position of the old field in the new resized one
   **/
  void resize(int w, int h, int pos_x = 0, int pos_y = 0)
  {
    *this = Field<T>(*this, w, h, pos_x, pos_y);
  }

  void clear()
  {
    width  = 0;
    height = 0;
    vec.clear();
  }

  /** Provides raw access to the underlying vector */
  std::vector<T>& get_vector() { return vec; }

  iterator begin () { return vec.begin (); }
  iterator end () { return vec.end (); }

  int size() const { return static_cast<int>(vec.size()); }
  int get_width () const { return width; }
  int get_height () const { return height; }
  geom::isize get_size() const { return {width, height}; }
};

#endif

/* EOF */
