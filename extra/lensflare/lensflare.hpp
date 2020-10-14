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

#ifndef HEADER_WINDSTILLE_EXTRA_LENSEFLAIR_LENSEFLAIR_HPP
#define HEADER_WINDSTILLE_EXTRA_LENSEFLAIR_LENSEFLAIR_HPP

#include <vector>

#include <glm/glm.hpp>
#include <geom/geom.hpp>

#include "display/surface.hpp"
#include "display/color.hpp"

struct Flair
{
  SurfacePtr m_surface;
  float      m_distance;
  float      m_scale;
  Color      m_color;

  Flair(SurfacePtr surface,
        float      distance,
        float      scale,
        Color color) :
    m_surface(surface),
    m_distance(distance),
    m_scale(scale),
    m_color(color)
  {}
};

class Lensflare
{
private:
  geom::isize m_aspect_ratio;
  geom::isize m_window_size;
  bool m_fullscreen;
  bool m_loop;

  SurfacePtr m_light;
  SurfacePtr m_lightquery;
  SurfacePtr m_superlight;
  SurfacePtr m_flair1;
  SurfacePtr m_flair2;
  SurfacePtr m_cover;
  SurfacePtr m_halo;

  typedef std::vector<Flair> Flairs;
  Flairs m_flairs;

  glm::vec2 m_mouse;

public:
  Lensflare();

  int run();

  void process_input();
  void draw();

private:
  Lensflare(const Lensflare&);
  Lensflare& operator=(const Lensflare&);
};

#endif

/* EOF */
