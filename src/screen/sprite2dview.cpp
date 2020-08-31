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

#include "screen/sprite2dview.hpp"

#include <filesystem>

#include "app/console.hpp"
#include "display/surface_manager.hpp"
#include "input/controller.hpp"
#include "util/directory.hpp"

extern std::vector<std::string> arg_files;

#define DISPLAY_W 800
#define DISPLAY_H 600

Sprite2DView::Sprite2DView()
  : sc(),
    directory(),
    shuffle_directory(),
    mode(),
    zoom(),
    pos(),
    width(),
    height(),
    aspect(),
    index(),
    sprite(),
    new_sprite(),
    fadein(),
    scale(),
    offset(),
    display_time(),
    show_thumbnail(),
    ignore_delta(),
    shuffle(),
    auto_scroll()
{
  auto_scroll = false;
  index = 0;

  for(std::vector<std::string>::iterator i = arg_files.begin(); i != arg_files.end(); ++i)
  {
    if (std::filesystem::is_directory(i->c_str()))
    {
      adddir(Pathname(*i, Pathname::kSysPath));
    }
    else
    {
      directory.push_back(Pathname(*i, Pathname::kSysPath));
    }
  }

  shuffle_directory = directory;
  shuffle = false;

  //std::sort(shuffle_directory.begin(), shuffle_directory.end());
  //std::random_shuffle(shuffle_directory.begin(), shuffle_directory.end());

  offset = 0.0f;

  if (directory.size() > 1)
    mode = SLIDESHOW;
  else
    mode = SLIDESHOW;

  zoom = 1.0f;
  pos  = Vector2f(0,0);
  display_time = 0.0f;
  show_thumbnail = false;
  ignore_delta = false;

  next_image(0);
  sprite = new_sprite;
  new_sprite = Sprite();
}

void
Sprite2DView::adddir(const Pathname& dirname)
{
  Directory::List lst = Directory::read(dirname);

  for (Directory::List::iterator i = lst.begin(); i != lst.end(); ++i)
  {
    if (std::filesystem::is_directory(i->get_sys_path()))
    {
      adddir(*i);
    }
    else
    {
      directory.push_back(*i);
    }
  }
}

Sprite2DView::~Sprite2DView()
{

}

void
Sprite2DView::draw()
{
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  switch(mode)
  {
    case SLIDESHOW:
      if (aspect > 4.0/3.0)
      {
        sprite.draw(Vector2f(-offset, 0));
      }
      else
      {
        sprite.draw(Vector2f(0, -offset));
      }

      if (new_sprite)
      {
        new_sprite.draw(Vector2f(0,0));
      }

      if (show_thumbnail)
      {
        Sprite small = sprite;
        small.set_alpha(1.0f);
        float w  = small.get_width();
        float h = small.get_height();
        float s;
        if (w > h)
        {
          s = 125.0f / w;

          w  *= s;
          h *= s;
          small.set_scale(s);

          small.draw(Vector2f(DISPLAY_W - w,
                              DISPLAY_H - h));
        }
        else
        {
          s = 125.0f / h;

          w  *= s;
          h *= s;
          small.set_scale(s);

          small.draw(Vector2f(DISPLAY_W - w,
                              DISPLAY_H - h));
        }
      }
      break;

    case MANUAL:
      sprite.set_scale(zoom);
      sprite.draw(pos);
      break;
  }
}

void
Sprite2DView::update_slideshow(float delta, const Controller& controller)
{
  if (!new_sprite)
  {
    width  = sprite.get_width();
    height = sprite.get_height();
    aspect = width/height;

    if (aspect > 4.0/3.0)
    { // expand vertical
      float s = DISPLAY_H/height;
      width  *= s;
      height *= s;
      sprite.set_scale(s);

      if (offset - (width - DISPLAY_W) > 0)
      {
        offset = (width - DISPLAY_W);

        if (auto_scroll && display_time > 3.0f)
          next_image();
      }
      else
      {
        offset += delta * 50.0f + controller.get_axis_state(X_AXIS) * 200.0f * delta;
      }

      offset +=  controller.get_ball_state(MOUSE_MOTION_Y);
    }
    else
    { // expand horizontal
      float s = 800.0f/width;
      width  *= s;
      height *= s;
      sprite.set_scale(s);

      if (offset - (height - DISPLAY_H) > 0)
      {
        offset = (width - DISPLAY_H);

        if (auto_scroll && display_time > 3.0f)
          next_image();
      }
      else
      {
        offset += delta * 50.0f +   controller.get_axis_state(X_AXIS) * 200.0f * delta;
      }

      offset += controller.get_ball_state(MOUSE_MOTION_Y);
    }
  }

  if (controller.button_was_pressed(PRIMARY_BUTTON))
  {
    next_image();
  }
  else if (controller.button_was_pressed(SECONDARY_BUTTON))
  {
    prev_image();
  }
}

void
Sprite2DView::next_image(int i)
{
  if (directory.size() > 1)
  {
    if (new_sprite)
    {
      sprite = new_sprite;
      sprite.set_alpha(1.0f);
      new_sprite = Sprite();
      offset = 0;
      display_time = 0;
    }

    index = static_cast<unsigned int>(index + i) % static_cast<int>(directory.size());

    std::vector<Pathname> dir;

    if (shuffle)
      dir = shuffle_directory;
    else
      dir = directory;

    bool retry = false;

    do
    {
      try
      {
        new_sprite = Sprite(dir[index]);
        retry = false;
      }
      catch(std::exception& e)
      {
        // FIXME: won't work in combination with shuffle
        std::cout << "Error: " << e.what() << std::endl;
        std::cout << "Removing '" << directory[index] << "' from the list" << std::endl;
        directory.erase(directory.begin() + index);
        index = static_cast<unsigned int>(index) % static_cast<int>(directory.size());
        retry = true;
      }
    }
    while (retry);

    ignore_delta = true;
    fadein = 0.0f;
    prepare_sprite(new_sprite);
    ConsoleLog << index << ": " << directory[index] << std::endl;
  }

  SpriteManager::current()->cleanup();
  SurfaceManager::current()->cleanup();
}

void
Sprite2DView::prepare_sprite(Sprite& sprite_)
{
  float w = sprite_.get_width();
  float h = sprite_.get_height();
  float a = w/h;

  if (a > 4.0/3.0)
  { // expand vertical
    float s = DISPLAY_H/h;
    w *= s;
    h *= s;
    sprite_.set_scale(s);
  }
  else
  { // expand horizontal
    float s = DISPLAY_W/w;
    w *= s;
    h *= s;
    sprite_.set_scale(s);
  }
}

void
Sprite2DView::update_manual(float delta, const Controller& controller)
{
  pos.x += controller.get_axis_state(X_AXIS) * 100.0f * delta + controller.get_ball_state(MOUSE_MOTION_X);
  pos.y += controller.get_axis_state(Y_AXIS) * 100.0f * delta + controller.get_ball_state(MOUSE_MOTION_Y);

  if (controller.get_button_state(PRIMARY_BUTTON))
    zoom *= 1.0f + 0.3f * delta;

  if (controller.get_button_state(SECONDARY_BUTTON))
    zoom /= 1.0f + 0.3f * delta;

  sprite.update(delta);
}

void
Sprite2DView::update(float delta, const Controller& controller)
{
  if (ignore_delta)
  {
    ignore_delta = false;
    delta = 0.0f;
  }

  display_time += delta * 0.5f;

  switch(mode) {
    case SLIDESHOW:
      update_slideshow(delta, controller);
      break;
    case MANUAL:
      update_manual(delta, controller);
      break;
  }

  if (controller.button_was_pressed(PDA_BUTTON))
  {
    if (shuffle)
    {
      std::vector<Pathname>::iterator i = std::find(directory.begin(), directory.end(),
                                                    shuffle_directory[index]);
      if (i != directory.end())
      {
        index = static_cast<int>(i - directory.begin());
      }
    }
    else
    {
      std::vector<Pathname>::iterator i = std::find(shuffle_directory.begin(), shuffle_directory.end(),
                                                    directory[index]);
      if (i != shuffle_directory.end())
      {
        index = static_cast<int>(i - shuffle_directory.begin());
      }
    }

    shuffle = !shuffle;

    std::cout << shuffle << " " << index << std::endl;
  }

  //  if (controller.button_was_pressed(INVENTORY_BUTTON))
  //std::random_shuffle(shuffle_directory.begin(), shuffle_directory.end());

  if (controller.button_was_pressed(TERTIARY_BUTTON))
    show_thumbnail = !show_thumbnail;

  if (controller.button_was_pressed(AIM_BUTTON))
  {
    if (mode == SLIDESHOW) mode = MANUAL;
    else if (mode == MANUAL) mode = SLIDESHOW;
  }

  if (new_sprite)
  {
    fadein += delta;

    if (fadein > 1.0f)
    {
      sprite = new_sprite;
      sprite.set_alpha(1.0f);
      new_sprite = Sprite();
      offset = 0;
      display_time = 0;
    }
    else
    {
      new_sprite.set_alpha(fadein);
    }
  }
}

void
Sprite2DView::set_sprite(const Pathname& filename)
{
  sprite = Sprite(filename);
  ignore_delta = true;
}

/* EOF */
