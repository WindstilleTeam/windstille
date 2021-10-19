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

#include "slideshow/slide_show.hpp"

#include <iostream>

#include <wstdisplay/surface_manager.hpp>
#include <wstdisplay/texture_manager.hpp>
#include "slideshow/slide_parser.hpp"

using namespace wstdisplay;

SlideShow::SlideShow(TextureManager& texture_manager,
                     SurfaceManager& surface_manager) :
  m_texture_manager(texture_manager),
  m_surface_manager(surface_manager),
  m_objects(),
  m_length(0.0f)
{
}

void
SlideShow::draw(wstdisplay::GraphicsContext& gc, float time, bool verbose)
{
  bool cleanup = false;

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
      if (verbose)
      {
        std::cout << obj->get_filename() << std::endl;
      }

      obj->draw(gc, relative_time);
    }
    else if (time < obj->begin()-60.0f || obj->end()+60.0f < time)
    { // unload anything that 60 seconds away from the current time pos
      if (obj->unload())
      {
        cleanup = true;
      }
    }
  }

  if (cleanup)
  {
    m_surface_manager.cleanup();
    m_texture_manager.cleanup();
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
  return static_cast<int>(m_objects.size());
}

void
SlideShow::clear()
{
  m_objects.clear();
  m_length = 0.0f;
}

void
SlideShow::load(const std::string& filename, const geom::fsize& aspect)
{
  SlideParser slide_parser(*this, aspect, m_surface_manager);
  slide_parser.load_from_file(filename);
}

float
SlideShow::find_next(float time)
{
  float best_time = m_length;
  for(std::vector<SlideObjectPtr>::iterator i = m_objects.begin(); i != m_objects.end(); ++i)
  {
    if ((*i)->begin() > time && (*i)->begin() < best_time)
    {
      best_time = (*i)->begin();
    }
  }
  return best_time;
}

float
SlideShow::find_prev(float time)
{
  float best_time = 0.0f;

  // find the start of the current object
  for(std::vector<SlideObjectPtr>::iterator i = m_objects.begin(); i != m_objects.end(); ++i)
  {
    if ((*i)->begin() < time && (*i)->begin() > best_time)
    {
      best_time = (*i)->begin();
    }
  }

  if ((true) /* FIXME: not_last_object */ )
  {
    // find the start of the object before the current object
    time = best_time;
    best_time = 0.0f;
    for(std::vector<SlideObjectPtr>::iterator i = m_objects.begin(); i != m_objects.end(); ++i)
    {
      if ((*i)->begin() < time && (*i)->begin() > best_time)
      {
        best_time = (*i)->begin();
      }
    }
  }

  return best_time;
}

/* EOF */
