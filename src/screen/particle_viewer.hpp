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

#ifndef HEADER_WINDSTILLE_SCREEN_PARTICLE_VIEWER_HPP
#define HEADER_WINDSTILLE_SCREEN_PARTICLE_VIEWER_HPP

#include <memory>
#include <memory>

#include <wstdisplay/compositor.hpp>
#include <wstdisplay/scene_context.hpp>
#include <wstdisplay/scenegraph/scene_graph.hpp>
#include <wstgui/screen.hpp>
#include <wstgui/gui_manager.hpp>

#include "particles/particle_system.hpp"
#include "sprite2d/sprite.hpp"
#include "util/pathname.hpp"

namespace wstgui {
class Slider;
class TabComponent;
}

namespace windstille {

class ParticleSystemGUI;

class ParticleViewer : public wstgui::Screen
{
private:
  wstdisplay::Compositor compositor;
  wstdisplay::SceneContext sc;
  wstdisplay::SceneGraph   sg;
  typedef std::vector<std::shared_ptr<ParticleSystem> > Systems;
  Systems systems;
  Sprite background;

  glm::vec2 pos;

  std::shared_ptr<wstdisplay::FillScreenPatternDrawable> m_background_drawable;
  std::shared_ptr<wstdisplay::FillScreenDrawable>        m_color_fill_drawable;

public:
  ParticleViewer();
  ~ParticleViewer() override;

  void draw(wstdisplay::GraphicsContext& gc) override;
  void update(float delta, wstinput::Controller const& controller) override;
  void load(Pathname const& filename);

private:
  ParticleViewer (ParticleViewer const&);
  ParticleViewer& operator= (ParticleViewer const&);
};

} // namespace windstille

#endif

/* EOF */
