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

#ifndef HEADER_WINDSTILLE_SCREEN_SPRITE2DVIEW_HPP
#define HEADER_WINDSTILLE_SCREEN_SPRITE2DVIEW_HPP

#include <wstdisplay/scene_context.hpp>
#include "screen/screen.hpp"
#include "sprite2d/sprite.hpp"
#include "util/pathname.hpp"

/**
 * A simple class to view 3d sprites and their different actions,
 * mostly usefull for debugging
 */
class Sprite2DView : public Screen
{
private:
  wstdisplay::SceneContext sc;
  std::vector<Pathname> directory;
  std::vector<Pathname> shuffle_directory;

  enum Mode { SLIDESHOW, MANUAL } mode;

  // Manual Mode Variables
  float  zoom;
  glm::vec2 pos;

  // Slideshow variables
  float width;
  float height;
  float aspect;

  int    index;
  Sprite sprite;

  Sprite new_sprite;
  float fadein;

  float  scale;
  float  offset;

  float display_time;
  bool show_thumbnail;
  bool ignore_delta;
  bool shuffle;

  bool auto_scroll;

public:
  Sprite2DView();
  ~Sprite2DView() override;

  void adddir(const Pathname& dir);

  void draw(wstdisplay::GraphicsContext& gc) override;
  void update(float delta, const Controller& controller) override;

  void update_slideshow(float delta, const Controller& controller);
  void update_manual(float delta, const Controller& controller);

  /** move \a i images forward in the directory */
  void next_image(int i = 1);
  void prev_image(int i = 1) { next_image(-i); }

  void set_sprite(const Pathname& filename);
  void prepare_sprite(Sprite& sprite);

private:
  Sprite2DView (const Sprite2DView&);
  Sprite2DView& operator= (const Sprite2DView&);
};

#endif

/* EOF */
