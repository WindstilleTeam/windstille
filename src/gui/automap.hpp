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

#ifndef HEADER_WINDSTILLE_GUI_AUTOMAP_HPP
#define HEADER_WINDSTILLE_GUI_AUTOMAP_HPP

#include "display/surface.hpp"
#include "component.hpp"

namespace gui {

/** */
class Automap : public Component
{
private:
  Surface surface;
  Vector  pos;
  float   zoom;

public:
  Automap(const lisp::Lisp* lisp, Component* parent);
  Automap(const Rectf& rect, Component* parent);
  ~Automap();

  void draw();
  void update(float delta, const Controller& controller);

private:
  Automap (const Automap&);
  Automap& operator= (const Automap&);
};

} // namespace gui

#endif

/* EOF */
