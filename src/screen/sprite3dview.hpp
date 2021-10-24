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

#ifndef HEADER_WINDSTILLE_SCREEN_SPRITE3DVIEW_HPP
#define HEADER_WINDSTILLE_SCREEN_SPRITE3DVIEW_HPP

#include <wstgui/screen.hpp>
#include <wstdisplay/compositor.hpp>

#include "sprite3d/sprite3d.hpp"
#include "util/pathname.hpp"

/**
 * A simple class to view 3d sprites and their different actions,
 * mostly usefull for debugging
 */
class Sprite3DView : public wstgui::Screen
{
private:
  wstdisplay::Compositor m_compositor;
  wstdisplay::SceneContext m_sc;
  Sprite3D m_sprite;
  std::vector<std::string> m_actions;
  int m_current_action;

  glm::quat m_rotation;
  float m_scale;

public:
  Sprite3DView();
  ~Sprite3DView() override;

  void draw(wstdisplay::GraphicsContext& gc) override;
  void update(float delta, wstinput::Controller const& controller) override;

  void set_model(const Pathname& filename);

  void handle_event(const SDL_Event& ) override;

private:
  Sprite3DView (const Sprite3DView&);
  Sprite3DView& operator= (const Sprite3DView&);
};

#endif

/* EOF */
