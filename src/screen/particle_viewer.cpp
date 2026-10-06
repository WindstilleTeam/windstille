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

#include "screen/particle_viewer.hpp"

#include "display/scene_context.hpp"

#include <stdexcept>
#include <sstream>

#include <logmich/log.hpp>

#include <wstinput/controller.hpp>
#include <wstdisplay/canvas.hpp>
#include <wstdisplay/opengl_window.hpp>
#include <wstdisplay/texture_manager.hpp>
#include <wstdisplay/texture_params.hpp>

#include "app/app.hpp"
#include "app/controller_def.hpp"
#include "app/menu_manager.hpp"
#include "util/pathname.hpp"

namespace windstille {

ParticleViewer::ParticleViewer()
  : sc(),
    m_view(),
    systems(),
    m_background(g_app.texture().get(Pathname("images/greychess.png"),
                                     wstdisplay::TextureParams{
                                       .wrap_x = wstdisplay::TextureWrap::Repeat,
                                       .wrap_y = wstdisplay::TextureWrap::Repeat
                                     })),
    pos()
{
}

ParticleViewer::~ParticleViewer()
{
}

void
ParticleViewer::load(Pathname const& filename)
{
  std::cout << "ParticleViewer: loading " << filename << std::endl;

  // Cleanup
  systems.clear();

  ReaderDocument const& doc = ReaderDocument::from_file(filename.get_sys_path());
  if (doc.get_name() != "particle-systems") {
    std::ostringstream msg;
    msg << "'" << filename << "' is not a particle-system file";
    throw std::runtime_error(msg.str());
  }

  ReaderMapping const& reader = doc.get_mapping();

  ReaderCollection particlesys_col;
  reader.read("particle-systems", particlesys_col);

  for (ReaderObject const& item : particlesys_col.get_objects()) {
    if (item.get_name() == "particle-system") {
      systems.push_back(std::shared_ptr<ParticleSystem>(new ParticleSystem(item.get_mapping(), g_app.surface())));
    } else {
      log_error("unknown particle-system: {}", item.get_name());
    }
  }

  std::cout << systems.size() << " particle systems ready to go" << std::endl;
}

void
ParticleViewer::draw(wstdisplay::Canvas& /*canvas*/)
{
  geom::fsize const size(g_app.window().get_drawable_size());

  {
    wstdisplay::Canvas& color = sc.color();
    wstdisplay::Canvas::Scope scope(color);
    color.set_z(-1000.0f);
    color.set_space(wstdisplay::Space::Screen);
    color.fill_pattern(m_background, geom::frect(geom::fpoint(0.0f, 0.0f), size), geom::foffset(pos.x, pos.y));
  }

  sc.light().fill_screen(surf::Color(0.4f, 0.4f, 0.4f));

  for(Systems::iterator i = systems.begin(); i != systems.end(); ++i)
  {
    (*i)->draw(sc);
  }
}

void
ParticleViewer::render(wstdisplay::Renderer& renderer)
{
  m_view.set_size(g_app.window().get_drawable_size());
  m_view.set_pos(geom::fpoint(-pos.x, -pos.y));
  sc.render(renderer, m_view);
}

void
ParticleViewer::update(float delta, wstinput::Controller const& controller)
{
  for(Systems::iterator i = systems.begin(); i != systems.end(); ++i)
    (*i)->update(delta);

  pos.x -= controller.get_axis_state(X_AXIS) * delta * 400.0f;
  pos.y -= controller.get_axis_state(Y_AXIS) * delta * 400.0f;

  if (controller.button_was_pressed(PAUSE_BUTTON) ||
      controller.button_was_pressed(ESCAPE_BUTTON))
  {
    MenuManager::display_pause_menu();
  }
}

} // namespace windstille

/* EOF */
