/*  $Id: windstille.hpp 1460 2007-06-18 04:03:31Z grumbel $
**   __      __ __             ___        __   __ __   __
**  /  \    /  \__| ____    __| _/_______/  |_|__|  | |  |   ____
**  \   \/\/   /  |/    \  / __ |/  ___/\   __\  |  | |  | _/ __ \
**   \        /|  |   |  \/ /_/ |\___ \  |  | |  |  |_|  |_\  ___/
**    \__/\  / |__|___|  /\____ /____  > |__| |__|____/____/\___  >
**         \/          \/      \/    \/                         \/
**  Copyright (C) 2005,2007 Matthias Braun <matze@braunis.de>,
**                          Ingo Ruhnke <grumbel@gmx.de>
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

#ifndef __SPRITE_DATA_HPP__
#define __SPRITE_DATA_HPP__

#include <string>
#include <vector>
#include <GL/glew.h>
#include <GL/gl.h>
#include "file_reader.hpp"
#include "math/vector.hpp"
#include "display/surface.hpp"
#include "ref.hpp"

namespace sprite2d
{
struct Action;

class Data
{
private:
  void parse(const std::string& dir, FileReader& lisp);
  
  Action* parse_action(const std::string& dir, FileReader& reader);
  void parse_images(Action* action, const std::string& dir, 
                    FileReader& reader);
  void parse_image_grid(Action* action, const std::string& dir,
                        FileReader& reader); 

public:
  Data(const std::string& filename);
  virtual ~Data();

  typedef std::vector<Action*> Actions;
  Actions actions;
};

struct Action
{
  typedef std::vector<Surface> Surfaces;

  std::string name;
  float       speed;
  Vector      offset;
  float       scale;
  Surfaces    surfaces;
};

}

#endif

