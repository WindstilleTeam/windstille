/*
**  Windstille - A Sci-Fi Action-Adventure Game
**  Copyright (C) 2005 Ingo Ruhnke <grumbel@gmail.com>
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

#ifndef HEADER_WINDSTILLE_PARTICLES_DEFORM_DRAWER_HPP
#define HEADER_WINDSTILLE_PARTICLES_DEFORM_DRAWER_HPP

#include <wstdisplay/drawing_context.hpp>
#include <wstdisplay/framebuffer.hpp>
#include <wstdisplay/shader_program.hpp>
#include <wstdisplay/surface.hpp>
#include <wstdisplay/surface_manager.hpp>
#include "particles/drawer.hpp"
#include "util/file_reader.hpp"

class ParticleSystem;

/**
 * DeformDrawer is similar to the SurfaceDrawer, it however doesn't
 * draw the surfaces to the screen but to a seperate framebuffer which
 * is then used as deform map over the screen, so it can be used for
 * heat effects from fire and such.
 */
class DeformDrawer : public Drawer
{
private:
  wstdisplay::FramebufferPtr framebuffer;
  wstdisplay::SurfacePtr     surface;
  wstdisplay::ShaderProgramPtr shader_program;

public:
  DeformDrawer(ReaderMapping const& props,
               wstdisplay::SurfaceManager& surface_manager);
  ~DeformDrawer() override;

  void draw(wstdisplay::DrawingContext& sc, ParticleSystem& psys);
  void draw(wstdisplay::GraphicsContext& gc, ParticleSystem const& /*psys*/) const override {}
private:
  DeformDrawer (DeformDrawer const&);
  DeformDrawer& operator= (DeformDrawer const&);
};

#endif

/* EOF */
