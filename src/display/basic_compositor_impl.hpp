/*
**  Windstille - A Sci-Fi Action-Adventure Game
**  Copyright (C) 2009 Ingo Ruhnke <grumbel@gmx.de>
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

#ifndef HEADER_WINDSTILLE_DISPLAY_BASIC_COMPOSITOR_IMPL_HPP
#define HEADER_WINDSTILLE_DISPLAY_BASIC_COMPOSITOR_IMPL_HPP

#include "display/compositor_impl.hpp"
#include "display/surface.hpp"

class BasicCompositorImpl : public CompositorImpl
{
private:
  Surface m_lightmap;

public:
  BasicCompositorImpl(const Size& window, const Size& viewport);

  void render(SceneContext& sc, SceneGraph* sg, const GraphicContextState& gc_state);
  
private:
  BasicCompositorImpl(const BasicCompositorImpl&);
  BasicCompositorImpl& operator=(const BasicCompositorImpl&);
};

#endif

/* EOF */
