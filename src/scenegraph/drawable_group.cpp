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

#include "scenegraph/drawable_group.hpp"

#include "scenegraph/drawable.hpp"

DrawableGroup::DrawableGroup()
  : Drawable(Vector2f(), 0.0f, glm::mat4(1.0)),
    m_drawables()
{
}

void
DrawableGroup::add_drawable(boost::shared_ptr<Drawable> drawable)
{
  m_drawables.push_back(drawable);
}

void
DrawableGroup::remove_drawable(boost::shared_ptr<Drawable> drawable)
{
  m_drawables.erase(std::remove(m_drawables.begin(), m_drawables.end(), drawable), m_drawables.end());
}
  
void
DrawableGroup::clear()
{
  m_drawables.clear();
}

void
DrawableGroup::render(unsigned int mask)
{
  for(Drawables::iterator i = m_drawables.begin(); i != m_drawables.end(); ++i)
  {
    if ((*i)->get_render_mask() & mask)
      (*i)->render(mask);
  }
}

/* EOF */
