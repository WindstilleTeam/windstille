/*  $Id$
**   __      __ __             ___        __   __ __   __
**  /  \    /  \__| ____    __| _/_______/  |_|__|  | |  |   ____
**  \   \/\/   /  |/    \  / __ |/  ___/\   __\  |  | |  | _/ __ \
**   \        /|  |   |  \/ /_/ |\___ \  |  | |  |  |_|  |_\  ___/
**    \__/\  / |__|___|  /\____ /____  > |__| |__|____/____/\___  >
**         \/          \/      \/    \/                         \/
**  Copyright (C) 2000,2005 Ingo Ruhnke <grumbel@gmx.de>
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

#ifndef HEADER_VIEW_HXX
#define HEADER_VIEW_HXX

#include "camera.hpp"
#include "graphic_context_state.hpp"
#include "math/vector.hpp"

class SceneContext;

/** This class is the gui component which renders the world to the
    screen */
class View
{
private:
  GraphicContextState state;
  Camera camera;

  // debugging helpers
  float zoom;
  Vector transform;

public:
  View();

  GraphicContextState get_gc_state() { return state; }

  /** @return the rectangle which represents the currently visible
      area, everything outside of it doesn't have to be drawn */
  Rectf get_clip_rect();
  Vector screen_to_world(const Vector& point);

  void draw(SceneContext& gc);
  void update(float delta);

  static View* current() { return current_; }

protected:
  static View* current_;
};

#endif

/* EOF */
