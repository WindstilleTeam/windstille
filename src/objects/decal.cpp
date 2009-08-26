/*
**  Windstille - A Sci-Fi Action-Adventure Game
**  Copyright (C) 2009 Ingo Ruhnke <grumbel@gmx.de>
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

#include "decal.hpp"

Decal::Decal()
  : m_surface(),
    m_scale(),
    m_angle(),
    m_hflip(),
    m_vflip()
{
  //m_drawable.reset(new Sprite3DDrawingRequest(&sprite, Vector2f(200, 600), 100.0f, Matrix::identity()));
  //Sector::current()->get_scene_graph().add_drawable(m_drawable);
}

Decal::~Decal()
{
}
  
void
Decal::draw(SceneContext& /*context*/)
{
}

void
Decal::update(float /*delta*/)
{
}

/* EOF */
