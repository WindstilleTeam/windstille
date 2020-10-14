/*
**  Windstille - A Sci-Fi Action-Adventure Game
**  Copyright (C) 2009 Ingo Ruhnke <grumbel@gmail.com>
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

#include "display/compositor.hpp"

#include <iostream>
#include <GL/glew.h>

#include "display/framebuffer_compositor_impl.hpp"
#include "display/basic_compositor_impl.hpp"

Compositor::Compositor(const geom::isize& window, const geom::isize& viewport) :
  impl()
{
  if (GLEW_ARB_framebuffer_object)
  {
    std::cout  << "Display:: framebuffer_object extension is supported" << std::endl;
    impl.reset(new FramebufferCompositorImpl(window, viewport));
  }
  else
  {
    std::cout  << "Display:: framebuffer_object extension is not supported" << std::endl;
    impl.reset(new BasicCompositorImpl(window, viewport));
  }
}

Compositor::~Compositor()
{
}

void
Compositor::render(SceneContext& sc, SceneGraph* sg, const GraphicContextState& state)
{
  impl->render(sc, sg, state);
}

/* EOF */
