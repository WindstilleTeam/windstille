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

#include <stdexcept>
#include <sstream>

#include <logmich/log.hpp>

#include <wstinput/controller.hpp>

#include "app/app.hpp"
#include "app/controller_def.hpp"
#include "app/menu_manager.hpp"
#include <wstdisplay/graphic_context_state.hpp>
#include <wstdisplay/graphics_context.hpp>
#include <wstdisplay/opengl_window.hpp>
#include <wstdisplay/texture_manager.hpp>
#include "particles/particle_system_drawable.hpp"
#include <wstdisplay/scenegraph/fill_screen_drawable.hpp>
#include <wstdisplay/scenegraph/fill_screen_pattern_drawable.hpp>
#include "util/pathname.hpp"

ParticleViewer::ParticleViewer()
  : compositor(g_app.window().get_size(), g_app.window().get_gc().size()),
    sc(),
    sg(),
    systems(),
    background(g_app.sprite().create(Pathname("images/greychess.sprite"))),
    pos(),
    m_background_drawable(),
    m_color_fill_drawable()
{
}

ParticleViewer::~ParticleViewer()
{
}

void
ParticleViewer::load(const Pathname& filename)
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

  {
    // Build the SceneGraph
    TexturePtr pattern_texture = g_app.texture().get(Pathname("images/greychess.png"));
    pattern_texture->set_wrap(GL_REPEAT);

    m_background_drawable.reset(new FillScreenPatternDrawable(pattern_texture, glm::vec2()));
    m_color_fill_drawable.reset(new FillScreenDrawable(RGBAf(0.4f, 0.4f, 0.4f)));

    m_background_drawable->set_render_mask(SceneContext::COLORMAP);
    m_color_fill_drawable->set_render_mask(SceneContext::LIGHTMAP);

    sg.add_drawable(m_background_drawable);
    sg.add_drawable(m_color_fill_drawable);

    for(Systems::iterator i = systems.begin(); i != systems.end(); ++i)
    {
      sg.add_drawable(std::shared_ptr<Drawable>(new ParticleSystemDrawable(**i)));
    }
  }
}

void
ParticleViewer::draw(GraphicsContext& gc)
{
  m_background_drawable->set_offset(pos);

  GraphicContextState state(gc.size().width(), gc.size().height());
  state.set_pos(-pos);
  compositor.render(gc, sc, &sg, state);
}

void
ParticleViewer::update(float delta, const Controller& controller)
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

/* EOF */
