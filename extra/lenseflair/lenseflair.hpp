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

#ifndef HEADER_WINDSTILLE_EXTRA_LENSEFLAIR_LENSEFLAIR_HPP
#define HEADER_WINDSTILLE_EXTRA_LENSEFLAIR_LENSEFLAIR_HPP

#include <vector>

#include "math/vector2f.hpp"
#include "math/size.hpp"
#include "display/surface.hpp"

struct Flair
{
  SurfacePtr m_surface;
  float      m_distance;
  float      m_scale;

  Flair(SurfacePtr surface,
        float      distance,
        float      scale) :
    m_surface(surface),
    m_distance(distance),
    m_scale(scale)
  {}
};

class Lenseflair
{
private:
  Size m_aspect_ratio;
  Size m_window_size;
  bool m_fullscreen;
  bool m_loop;

  SurfacePtr m_light;
  SurfacePtr m_superlight;
  SurfacePtr m_flair1;
  SurfacePtr m_flair2;
  SurfacePtr m_cover;
  SurfacePtr m_halo;

  typedef std::vector<Flair> Flairs;
  Flairs m_flairs;

  Vector2f m_mouse;
  
public:
  Lenseflair();
  
  int main();

  void process_input();
  void draw();

private:
  Lenseflair(const Lenseflair&);
  Lenseflair& operator=(const Lenseflair&);
};

#endif

/* EOF */
