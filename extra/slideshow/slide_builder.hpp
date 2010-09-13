/*
**  Windstille - A Sci-Fi Action-Adventure Game
**  Copyright (C) 2010 Ingo Ruhnke <grumbel@gmx.de>
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

#ifndef HEADER_WINDSTILLE_EXTRA_SLIDESHOW_SLIDE_BUILDER_HPP
#define HEADER_WINDSTILLE_EXTRA_SLIDESHOW_SLIDE_BUILDER_HPP

#include <string>
#include <vector>
#include <map>

#include "math/size.hpp"
#include "slideshow/slide_object.hpp"
#include "slideshow/slide_path.hpp"

class SlideShow;

class SlideBuilder
{
private:

public:
  SlideBuilder();

private:
  SlideBuilder(const SlideBuilder&);
  SlideBuilder& operator=(const SlideBuilder&);
};

#endif

/* EOF */
