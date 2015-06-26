/*
**  Windstille - A Sci-Fi Action-Adventure Game
**  Copyright (C) 2010 Ingo Ruhnke <grumbel@gmail.com>
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

#ifndef HEADER_WINDSTILLE_EXTRA_SLIDESHOW_SLIDE_SHOW_HPP
#define HEADER_WINDSTILLE_EXTRA_SLIDESHOW_SLIDE_SHOW_HPP

#include <vector>

#include "slideshow/slide_object.hpp"

class SlideShow
{
private:
  std::vector<SlideObjectPtr> m_objects;
  float m_length;

public:
  SlideShow();

  void draw(float time, bool verbose);

  bool done(float time) const;

  float length() const { return m_length; }
  int size() const;

  void add(SlideObjectPtr object);
  void clear();

  void load(const std::string& filename, const Sizef& aspect);

  /** search for the start time of the next SlideObject, starting from time */
  float find_next(float time);
  float find_prev(float time);

private:
  SlideShow(const SlideShow&);
  SlideShow& operator=(const SlideShow&);
};

#endif

/* EOF */
