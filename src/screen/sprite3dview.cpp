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

#include "screen/sprite3dview.hpp"

#include <wstinput/controller.hpp>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include <glm/gtx/quaternion.hpp>

#include "app/app.hpp"
#include "app/controller_def.hpp"
#include "app/menu_manager.hpp"
#include <wstdisplay/graphics_context.hpp>
#include <wstdisplay/graphic_context_state.hpp>
#include <wstdisplay/opengl_window.hpp>
#include "font/fonts.hpp"
#include "sprite3d/manager.hpp"
#include "util/pathname.hpp"

Sprite3DView::Sprite3DView() :
  m_compositor(g_app.window().get_size(), g_app.window().get_gc().size()),
  m_sc(),
  m_sprite(),
  m_actions(),
  m_current_action(0),
  m_rotation(1.0f, 0.0f, 0.0f, 0.0f),
  m_scale(2.0f)
{
  m_sprite = g_app.sprite3d().create(Pathname("models/characters/jane/jane.wsprite"));
  m_actions = m_sprite.get_actions();
  m_sprite.set_action(m_actions[m_current_action]);
}

Sprite3DView::~Sprite3DView()
{
}

void
Sprite3DView::set_model(const Pathname& filename)
{
  m_sprite = g_app.sprite3d().create(filename);
  m_actions = m_sprite.get_actions();
}

void
Sprite3DView::draw(GraphicsContext& gc)
{
  m_sc.reset_modelview();
  //sc.translate(-config->screen_width/2, -config->screen_height/2);
  //sc.scale(2.0f, 2.0f);

  m_sc.color().fill_screen(RGBAf(0.5, 0.0, 0.5));

  m_sc.push_modelview();
  m_sc.translate(static_cast<float>(gc.size().width()) / 2.0f,
                 static_cast<float>(gc.size().height()) / 2.0f);
  m_sc.scale(m_scale, m_scale);

  // Rotate
  m_sc.mult_modelview(glm::mat4_cast(m_rotation));
  m_sc.translate(0, 64.0f); // FIXME: use object height/2 instead of 64
  m_sprite.draw(m_sc.color(), glm::vec2(0,0), 0);
  m_sc.pop_modelview();

  //glm::mat4 matrix = m_sc.color().get_modelview();
  //matrix.translate(-gc.size().width()/2, -gc.size().height()/2, 0);
  //sprite.draw(m_sc.color(), matrix, 0.0f);

  m_sc.light().fill_screen(RGBAf(1.0, 1.0, 1.0));
  //sc.color().draw("Hello World", 100, 100);
  m_compositor.render(gc, m_sc, nullptr, GraphicContextState(gc.size().width(),
                                                             gc.size().height()));

  float x = 10.0f;
  float y =  static_cast<float>(g_app.fonts().vera12->get_height()) + 5.0f;
  int line_height = g_app.fonts().vera12->get_height()+5;

  for(int i = 0; i < int(m_actions.size()); ++i)
  {
    if (i == m_current_action)
      g_app.fonts().vera12->draw(gc,
                                 glm::vec2(x, y),
                                 m_actions[i], RGBAf(1.0f, 1.0f, 1.0f));
    else
      g_app.fonts().vera12->draw(gc,
                                 glm::vec2(x, y),
                                 m_actions[i], RGBAf(0.7f, 0.7f, 0.7f));

    y += static_cast<float>(line_height);
    if (y > 580.0f)
    {
      x += 200.0f;
      y =  static_cast<float>(g_app.fonts().vera12->get_height()) + 5.0f;
    }
  }
}

void
Sprite3DView::update(float delta, const Controller& controller)
{
  m_sprite.update(delta);
  //std::cout << "Delta: " << delta << std::endl;

  int last_action = m_current_action;
  if (controller.button_was_pressed(MENU_DOWN_BUTTON)) {
    if (m_current_action == int(m_actions.size()) - 1) {
      m_current_action = 0;
    } else {
      m_current_action += 1;
    }
  } else if (controller.button_was_pressed(MENU_UP_BUTTON)) {
    if (m_current_action == 0) {
      m_current_action = static_cast<int>(m_actions.size()) - 1;
    } else {
      m_current_action -= 1;
    }
  }

  if (controller.get_button_state(RIGHT_SHOULDER_BUTTON)) {
    m_scale *= 1.0f + 0.6f * delta;
  } else if (controller.get_button_state(LEFT_SHOULDER_BUTTON)) {
    m_scale /= 1.0f + 0.6f * delta;
  }

  if (last_action != m_current_action && !m_actions.empty()) {
    m_sprite.set_action(m_actions[m_current_action]);
  }

  m_rotation = glm::quat(controller.get_axis_state(X2_AXIS) * delta * 4.0f,
                         glm::vec3(0.0f, 1.0f, 0.0f)) * m_rotation;
  m_rotation = glm::quat(controller.get_axis_state(Y2_AXIS) * delta * 4.0f,
                         glm::vec3(1.0f, 0.0f, 0.0f)) * m_rotation;
  m_rotation = glm::quat(-controller.get_axis_state(X_AXIS) * delta * 4.0f,
                         glm::vec3(0.0f, 0.0f, 1.0f)) * m_rotation;
  m_rotation = glm::normalize(m_rotation);

  if (controller.get_button_state(VIEW_CENTER_BUTTON))
  {
    m_rotation = glm::quat(1.0f, 0.0f, 0.0f, 0.0f);
  }

  if (controller.button_was_pressed(ESCAPE_BUTTON) ||
      controller.button_was_pressed(PAUSE_BUTTON)) {
    MenuManager::display_pause_menu();
  }
}

void
Sprite3DView::handle_event(const SDL_Event& )
{

}

/* EOF */
