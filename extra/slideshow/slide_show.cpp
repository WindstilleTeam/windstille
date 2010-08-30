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

#include "slideshow/slide_show.hpp"

#include "slideshow/slide_builder.hpp"

SlideShow::SlideShow() :
  m_objects(),
  m_length(0.0f)
{
}

void
SlideShow::draw(float time)
{
  for(std::vector<SlideObjectPtr>::iterator i = m_objects.begin(); i != m_objects.end(); ++i)
  {
    SlideObjectPtr& obj = *i;

    //std::cout << obj.get() << ": " << obj->begin() << " " << obj->length() << std::endl;

    if (obj->begin() <= time && time < (obj->begin() + obj->length()))
    {
      //std::cout << "draw" << std::endl;
      // active object
      float relative_time = time - obj->begin();
      //std::cout << time << " " << relative_time << std::endl;
      obj->draw(relative_time);
    }
  }
}

void
SlideShow::add(SlideObjectPtr object)
{
  m_length = std::max(m_length, object->begin() + object->length());
  m_objects.push_back(object);
}

bool
SlideShow::done(float time) const
{
  return (time >= m_length);
}

int
SlideShow::size() const
{
  return m_objects.size();
}

void
SlideShow::clear()
{
  m_objects.clear();
  m_length = 0.0f;
}

void
SlideShow::load(const std::string& filename, const Sizef& aspect)
{
  SlideBuilder slide_builder(*this, aspect);
  slide_builder.load_from_file(filename);
}

/* EOF */
