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

#include "screen/sprite3dview.hpp"

#include "app/menu_manager.hpp"
#include "display/display.hpp"
#include "font/fonts.hpp"
#include "input/controller.hpp"
#include "display/graphic_context_state.hpp"
#include "display/opengl_window.hpp"

Sprite3DView::Sprite3DView()
  : compositor(OpenGLWindow::current()->get_size(), Display::get_size()),
    sc(),
    sprite(),
    actions(),
    current_action(),
    rotation(),
    scale()
{
  current_action = 0;

  sprite = Sprite3D(Pathname("models/characters/jane/jane.wsprite"));
  actions = sprite.get_actions();

  sprite.set_action(actions[current_action]);

  rotation = Quaternion();

  scale = 2.0f;
}

Sprite3DView::~Sprite3DView()
{
}

void
Sprite3DView::set_model(const Pathname& filename)
{
  sprite  = Sprite3D(filename);
  actions = sprite.get_actions();
}

void
Sprite3DView::draw()
{
  sc.reset_modelview();
  //sc.translate(-config->screen_width/2, -config->screen_height/2);
  //sc.scale(2.0f, 2.0f);

  sc.color().fill_screen(Color(0.5, 0.0, 0.5));

  sc.push_modelview();
  sc.translate(static_cast<float>(Display::get_width())/2.0f, static_cast<float>(Display::get_height())/2.0f);
  sc.scale(scale, scale);

  // Rotate
  sc.mult_modelview(glm::mat4_cast(rotation));
  sc.translate(0, 64.0f); // FIXME: use object height/2 instead of 64
  sprite.draw(sc.color(), Vector2f(0,0), 0);
  sc.pop_modelview();

  //Matrix matrix = sc.color().get_modelview();
  //matrix.translate(-Display::get_width()/2, -Display::get_height()/2, 0);
  //sprite.draw(sc.color(), matrix, 0.0f);

  sc.light().fill_screen(Color(1.0, 1.0, 1.0));
  //sc.color().draw("Hello World", 100, 100);
  compositor.render(sc, 0, GraphicContextState(Display::get_width(),
                                               Display::get_height()));

  float x = 10.0f;
  float y =  static_cast<float>(Fonts::current()->vera12->get_height()) + 5.0f;
  int line_height = Fonts::current()->vera12->get_height()+5;

  for(int i = 0; i < int(actions.size()); ++i)
  {
    if (i == current_action)
      Fonts::current()->vera12->draw(Vector2f(x, y),
                                     actions[i], Color(1.0f, 1.0f, 1.0f));
    else
      Fonts::current()->vera12->draw(Vector2f(x, y),
                                     actions[i], Color(0.7f, 0.7f, 0.7f));

    y += static_cast<float>(line_height);
    if (y > 580.0f)
    {
      x += 200.0f;
      y =  static_cast<float>(Fonts::current()->vera12->get_height()) + 5.0f;
    }
  }
}

void
Sprite3DView::update(float delta, const Controller& controller)
{
  sprite.update(delta);
  //std::cout << "Delta: " << delta << std::endl;

  int last_action = current_action;
  if (controller.button_was_pressed(MENU_DOWN_BUTTON))
  {
    if (current_action == int(actions.size())-1)
      current_action = 0;
    else
      current_action += 1;
  }
  else if (controller.button_was_pressed(MENU_UP_BUTTON))
  {
    if (current_action == 0)
      current_action = static_cast<int>(actions.size()) - 1;
    else
      current_action -= 1;
  }

  if (controller.get_button_state(RIGHT_SHOULDER_BUTTON))
  {
    scale *= 1.0f + 0.6f * delta;
  }
  else if (controller.get_button_state(LEFT_SHOULDER_BUTTON))
  {
    scale /= 1.0f + 0.6f * delta;
  }

  if (last_action != current_action && !actions.empty())
  {
    sprite.set_action(actions[current_action]);
  }

  rotation = Quaternion(-controller.get_axis_state(X2_AXIS) * delta * 4.0f,
                        Vector3(0.0f, 1.0f, 0.0f)) * rotation;
  rotation = Quaternion(controller.get_axis_state(Y2_AXIS) * delta * 4.0f,
                        Vector3(1.0f, 0.0f, 0.0f)) * rotation;
  rotation = Quaternion(controller.get_axis_state(X_AXIS) * delta * 4.0f,
                        Vector3(0.0f, 0.0f, 1.0f)) * rotation;

  if (controller.get_button_state(VIEW_CENTER_BUTTON))
  {
    rotation = Quaternion();
  }

  if (controller.button_was_pressed(ESCAPE_BUTTON) ||
      controller.button_was_pressed(PAUSE_BUTTON))
  {
    MenuManager::display_pause_menu();
  }
}

void
Sprite3DView::handle_event(const SDL_Event& )
{

}

/* EOF */
