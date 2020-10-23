/*
**  Windstille - A Sci-Fi Action-Adventure Game
**  Copyright (C) 2005,2007 Matthias Braun <matze@braunis.de>,
**                          Ingo Ruhnke <grumbel@gmail.com>
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

#ifndef HEADER_WINDSTILLE_SPRITE2D_DATA_HPP
#define HEADER_WINDSTILLE_SPRITE2D_DATA_HPP

#include <GL/glew.h>

#include "util/file_reader.hpp"
#include <wstdisplay/surface.hpp>
#include <wstdisplay/surface_manager.hpp>

struct SpriteAction
{
  typedef std::vector<SurfacePtr> Surfaces;

  std::string name;
  float       speed;
  glm::vec2    offset;
  float       scale;
  Surfaces    surfaces;

public:
  SpriteAction()
    : name(),
      speed(),
      offset(),
      scale(),
      surfaces()
  {}
};

class SpriteData
{
private:
  void parse(std::filesystem::path const& dir, ReaderMapping const& lisp,
             SurfaceManager& surface_manager);
  SpriteAction* parse_action(std::filesystem::path const& dir, ReaderMapping const& reader,
                             SurfaceManager& surface_manager);

public:
  SpriteData(std::filesystem::path const& filename,
             SurfaceManager& surface_manager);
  virtual ~SpriteData();

  typedef std::vector<SpriteAction*> Actions;
  Actions actions;
};

#endif

/* EOF */
