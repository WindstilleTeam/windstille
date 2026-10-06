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
#include <surf/color.hpp>
#include <surf/software_surface.hpp>
#include <wstdisplay/fwd.hpp>
#include <wstdisplay/surface.hpp>

struct Flair
{
  wstdisplay::Surface m_surface;
  float      m_distance;
  float      m_scale;
  surf::Color      m_color;

  Flair(wstdisplay::Surface const& surface,
        float distance,
        float scale,
        surf::Color color) :
    m_surface(surface),
    m_distance(distance),
    m_scale(scale),
    m_color(color)
  {}
};

class Lensflare
{
private:
  /** The coordinate system everything is placed in, scaled to the window */
  geom::isize m_aspect_ratio;
  geom::isize m_window_size;
  bool m_fullscreen;
  bool m_loop;

  wstdisplay::Surface m_light;
  wstdisplay::Surface m_lightquery;
  wstdisplay::Surface m_superlight;
  wstdisplay::Surface m_flair1;
  wstdisplay::Surface m_flair2;
  wstdisplay::Surface m_cover;
  wstdisplay::Surface m_halo;

  /** For computing how much of the light the cover hides */
  surf::SoftwareSurface m_lightquery_image;
  surf::SoftwareSurface m_cover_image;
  glm::vec2 m_cover_pos;

  typedef std::vector<Flair> Flairs;
  Flairs m_flairs;

  glm::vec2 m_mouse;

public:
  Lensflare();

  int run();
  void process_input();
  void draw(wstdisplay::Canvas& canvas);

private:
  /** Fraction of the light that isn't hidden by the cover, in [0, 1] */
  float get_visibility() const;

private:
  Lensflare(const Lensflare&);
  Lensflare& operator=(const Lensflare&);
};

#endif

/* EOF */
