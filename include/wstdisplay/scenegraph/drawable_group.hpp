// Windstille Display Library
// Copyright (C) 2002-2020 Ingo Ruhnke <grumbel@gmail.com>
//
// This program is free software: you can redistribute it and/or modify it
// under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation, either version 3 of the License, or (at your
// option) any later version.
//
// This program is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
// or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public
// License for more details.
//
// You should have received a copy of the GNU Lesser General Public License
// along with this program. If not, see <http://www.gnu.org/licenses/>.

#ifndef HEADER_WINDSTILLE_SCENEGRAPH_DRAWABLE_GROUP_HPP
#define HEADER_WINDSTILLE_SCENEGRAPH_DRAWABLE_GROUP_HPP

#include <memory>
#include <vector>

#include <wstdisplay/scenegraph/drawable.hpp>

namespace wstdisplay {

class Texture;

class DrawableGroup : public Drawable
{
private:
  typedef std::vector<std::shared_ptr<Drawable> > Drawables;
  Drawables m_drawables;

public:
  DrawableGroup();

  void add_drawable(std::shared_ptr<Drawable> drawable);
  void remove_drawable(std::shared_ptr<Drawable> drawable);
  int  size() const { return static_cast<int>(m_drawables.size()); }

  void clear();

  void render(GraphicsContext& gc, unsigned int mask) override;

private:
  DrawableGroup(const DrawableGroup&);
  DrawableGroup& operator=(const DrawableGroup&);
};

} // namespace wstdisplay

#endif

/* EOF */
