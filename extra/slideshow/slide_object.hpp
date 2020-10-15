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

#ifndef HEADER_WINDSTILLE_EXTRA_SLIDESHOW_SLIDE_OBJECT_HPP
#define HEADER_WINDSTILLE_EXTRA_SLIDESHOW_SLIDE_OBJECT_HPP

#include <memory>

#include "display/surface.hpp"

#include "slideshow/slide_path.hpp"

class SurfaceManager;

class SlideObject
{
private:
  std::filesystem::path m_filename;
  SurfaceManager& m_surface_manager;
  geom::fsize m_size;
  SurfacePtr m_surface;
  float m_begin;
  SlidePath m_path;

  float m_fade_in_time;
  float m_fade_out_time;

public:
  SlideObject(const std::filesystem::path& filename,
              SurfaceManager& surface_manager);

  void draw(GraphicsContext& gc, float relative_time);

  float length() const;
  float begin() const;
  float end() const;

  void set_begin(float beg);
  void set_fade_in(float f);
  void set_fade_out(float f);

  SlidePath& get_path() { return m_path; }

  float get_width() const;
  float get_height() const;

  std::filesystem::path get_filename() const;

  bool unload();

private:
  SlideObject(const SlideObject&);
  SlideObject& operator=(const SlideObject&);
};

typedef std::shared_ptr<SlideObject> SlideObjectPtr;

#endif

/* EOF */
