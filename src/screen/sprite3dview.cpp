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

#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include <glm/gtx/quaternion.hpp>

#include "app/app.hpp"
#include "app/controller_def.hpp"
#include "app/menu_manager.hpp"
#include <wstdisplay/canvas.hpp>
#include <wstdisplay/opengl_window.hpp>
#include "font/fonts.hpp"
#include "sprite3d/manager.hpp"
#include "util/pathname.hpp"

namespace windstille {

Sprite3DView::Sprite3DView() :
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
Sprite3DView::set_model(Pathname const& filename)
{
  m_sprite = g_app.sprite3d().create(filename);
  m_actions = m_sprite.get_actions();
}

void
Sprite3DView::draw(wstdisplay::Canvas& canvas)
{
  canvas.fill_screen(surf::Color(0.5f, 0.0f, 0.5f));

  {
    wstdisplay::Canvas::Scope scope(canvas);
    canvas.translate(static_cast<float>(g_app.window().get_size().width()) / 2.0f,
                     static_cast<float>(g_app.window().get_size().height()) / 2.0f);

    // FIXME: use object height/2 instead of 64
    glm::mat4 model = glm::scale(glm::mat4(1.0f), glm::vec3(m_scale, m_scale, m_scale));
    model = model * glm::mat4_cast(m_rotation);
    model = glm::translate(model, glm::vec3(0.0f, 64.0f, 0.0f));
    m_sprite.draw(canvas, glm::vec2(0, 0), 0, model);
  }

  float x = 10.0f;
  float y = g_app.fonts().vera12->get_height() + 5.0f;
  float const line_height = g_app.fonts().vera12->get_height() + 5.0f;

  for(int i = 0; i < int(m_actions.size()); ++i)
  {
    if (i == m_current_action)
      canvas.draw_text(*g_app.fonts().vera12, glm::vec2(x, y),
                                 m_actions[i], surf::Color(1.0f, 1.0f, 1.0f));
    else
      canvas.draw_text(*g_app.fonts().vera12, glm::vec2(x, y),
                                 m_actions[i], surf::Color(0.7f, 0.7f, 0.7f));

    y += line_height;
    if (y > 580.0f)
    {
      x += 200.0f;
      y = g_app.fonts().vera12->get_height() + 5.0f;
    }
  }
}

void
Sprite3DView::update(float delta, wstinput::Controller const& controller)
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
Sprite3DView::handle_event(SDL_Event const& )
{

}

} // namespace windstille

/* EOF */
