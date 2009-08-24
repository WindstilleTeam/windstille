/*
**  Windstille - A Sci-Fi Action-Adventure Game
**  Copyright (C) 2005 Ingo Ruhnke <grumbel@gmx.de>
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

#ifndef HEADER_WINDSTILLE_SCREEN_PARTICLE_VIEWER_HPP
#define HEADER_WINDSTILLE_SCREEN_PARTICLE_VIEWER_HPP

#include <boost/scoped_ptr.hpp>
#include <boost/shared_ptr.hpp>
#include "screen/screen.hpp"
#include "sprite2d/sprite.hpp"
#include "gui/gui_manager.hpp"
#include "display/scene_context.hpp"
#include "particles/particle_system.hpp"

namespace gui {
class Slider;
class TabComponent;
}

class ParticleSystemGUI;

/** */
class ParticleViewer : public Screen
{
private:
  SceneContext sc;
  typedef std::vector<boost::shared_ptr<ParticleSystem> > Systems;
  Systems systems;
  Sprite background;

  Vector2f pos;

public:
  ParticleViewer();
  ~ParticleViewer();
  
  void draw();
  void update(float delta, const Controller& controller);
  void load(const Pathname& filename);

private:
  ParticleViewer (const ParticleViewer&);
  ParticleViewer& operator= (const ParticleViewer&);
};

#endif

/* EOF */
