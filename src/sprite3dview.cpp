/*  $Id$
**   __      __ __             ___        __   __ __   __
**  /  \    /  \__| ____    __| _/_______/  |_|__|  | |  |   ____
**  \   \/\/   /  |/    \  / __ |/  ___/\   __\  |  | |  | _/ __ \
**   \        /|  |   |  \/ /_/ |\___ \  |  | |  |  |_|  |_\  ___/
**    \__/\  / |__|___|  /\____ /____  > |__| |__|____/____/\___  >
**         \/          \/      \/    \/                         \/
**  Copyright (C) 2005 Ingo Ruhnke <grumbel@gmx.de>
**
**  This program is free software; you can redistribute it and/or
**  modify it under the terms of the GNU General Public License
**  as published by the Free Software Foundation; either version 2
**  of the License, or (at your option) any later version.
**
**  This program is distributed in the hope that it will be useful,
**  but WITHOUT ANY WARRANTY; without even the implied warranty of
**  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
**  GNU General Public License for more details.
** 
**  You should have received a copy of the GNU General Public License
**  along with this program; if not, write to the Free Software
**  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA
**  02111-1307, USA.
*/

#include <iostream>
#include "input/controller.hpp"
#include "display/display.hpp"
#include "console.hpp"
#include "font/ttf_font.hpp"
#include "font/fonts.hpp"
#include "screen_manager.hpp"
#include "sprite3dview.hpp"

Sprite3DView::Sprite3DView()
{
  current_action = 0;

  sprite = Sprite3D("models/characters/jane/jane.wsprite");
  actions = sprite.get_actions();

  sprite.set_action(actions[current_action]);

  rotx  = 0.0f;
  roty  = 0.0f;
  scale = 1.0f;
}

Sprite3DView::~Sprite3DView()
{
}

void
Sprite3DView::set_model(const std::string& filename)
{
  sprite  = Sprite3D(filename);
  actions = sprite.get_actions();
}

void
Sprite3DView::draw()
{
  sc.reset_modelview();
  //sc.translate(-config->screen_width/2, -config->screen_height/2 + 150);
  //sc.scale(2.0f, 2.0f);
  
  sc.color().fill_screen(Color(0.5, 0.0, 0.5));

  sc.push_modelview();
  sc.translate(Display::get_width()/2, Display::get_height()/2 + 200);
  sc.scale(3.0f, 3.0f);
  sc.rotate(rotx, 0.0f, 1.0f, 0.0f);
  sc.rotate(roty, 1.0f, 0.0f, 0.0f);
  sprite.draw(sc.color(), Vector(0,0), 0); 
  sc.pop_modelview();

  //Matrix matrix = sc.color().get_modelview();
  //matrix.translate(-Display::get_width()/2, -Display::get_height()/2, 0);
  //sprite.draw(sc.color(), matrix, 0.0f);

  sc.light().fill_screen(Color(1.0, 1.0, 1.0));
  //sc.color().draw("Hello World", 100, 100);
  sc.render();
  
  int x = 10;
  int y =  Fonts::vera12->get_height() + 5;
  int line_height = Fonts::vera12->get_height()+5;

  for(int i = 0; i < int(actions.size()); ++i)
    {
      if (i == current_action)
        Fonts::vera12->draw(x, y,
                               actions[i], Color(1.0f, 1.0f, 1.0f));
      else
        Fonts::vera12->draw(x, y,
                               actions[i], Color(0.7f, 0.7f, 0.7f));


      y += line_height;
      if (y > 580)
        {
          x += 200;
          y =  Fonts::vera12->get_height() + 5;
        }
    }
}

void
Sprite3DView::update(float delta, const Controller& controller)
{
  sprite.update(delta);
  //std::cout << "Delta: " << delta << std::endl;

  int last_action = current_action;
  if (controller.axis_was_pressed_up(Y_AXIS))
    {
      if (current_action == int(actions.size())-1)
        current_action = 0;
      else
        current_action += 1;
    }
  else if (controller.axis_was_pressed_down(Y_AXIS))
    {
      if (current_action == 0)
        current_action = actions.size()-1;
      else
        current_action -= 1;
    }

  if (last_action != current_action && !actions.empty())
    {
      sprite.set_action(actions[current_action]);
    }

  rotx += controller.get_axis_state(X2_AXIS) * 50.0f * delta;
  roty += controller.get_axis_state(Y2_AXIS) * 50.0f * delta;

  //std::cout << controller.get_axis_state(Y2_AXIS) << std::endl;

  Uint8 *keystate = SDL_GetKeyState(NULL);

  if(keystate[SDLK_ESCAPE])
    screen_manager.pop_screen();
}

/* EOF */
