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

#ifndef HEADER_WINDSTILLE_OBJECTS_SHOCKWAVE_HPP
#define HEADER_WINDSTILLE_OBJECTS_SHOCKWAVE_HPP

#include "display/shader_program.hpp"
#include "display/texture.hpp"
#include "engine/game_object.hpp"

/** */
class Shockwave : public GameObject
{
private:
  Vector        pos;
  Texture       noise;
  ShaderProgram shader_program;
  float radius;

public:
  Shockwave(FileReader& props);
  ~Shockwave();

  void draw (SceneContext& context);
  void update (float delta);

private:
  Shockwave (const Shockwave&);
  Shockwave& operator= (const Shockwave&);
};

#endif

/* EOF */
