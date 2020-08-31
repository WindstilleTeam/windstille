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

#include "app/menu_manager.hpp"
#include "display/display.hpp"
#include "display/opengl_window.hpp"
#include "display/graphic_context_state.hpp"
#include "scenegraph/particle_system_drawable.hpp"
#include "scenegraph/fill_screen_pattern_drawable.hpp"
#include "scenegraph/fill_screen_drawable.hpp"
#include "input/controller.hpp"
#include "util/sexpr_file_reader.hpp"

ParticleViewer::ParticleViewer()
  : compositor(OpenGLWindow::current()->get_size(), Display::get_size()),
    sc(),
    sg(),
    systems(),
    background(Pathname("images/greychess.sprite")),
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

  FileReader root_reader = FileReader::parse(filename);
  if(root_reader.get_name() != "particle-systems")
  {
    std::ostringstream msg;
    msg << "'" << filename << "' is not a particle-system file";
    throw std::runtime_error(msg.str());
  }

  std::vector<FileReader> sections = root_reader.get_sections();
  for(std::vector<FileReader>::iterator i = sections.begin(); i != sections.end(); ++i)
  {
    if (i->get_name() == "particle-system")
    {
      systems.push_back(std::shared_ptr<ParticleSystem>(new ParticleSystem(*i)));
    }
  }

  std::cout << systems.size() << " particle systems ready to go" << std::endl;

  {
    // Build the SceneGraph
    TexturePtr pattern_texture = Texture::create(Pathname("images/greychess.png"));
    pattern_texture->set_wrap(GL_REPEAT);

    m_background_drawable.reset(new FillScreenPatternDrawable(pattern_texture, Vector2f()));
    m_color_fill_drawable.reset(new FillScreenDrawable(Color(0.4f, 0.4f, 0.4f)));

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
ParticleViewer::draw()
{
  m_background_drawable->set_offset(pos);

  GraphicContextState state(Display::get_width(), Display::get_height());
  state.set_pos(-pos);
  compositor.render(sc, &sg, state);
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
