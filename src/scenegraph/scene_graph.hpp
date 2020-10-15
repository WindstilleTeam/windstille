/*
**  Windstille - A Sci-Fi Action-Adventure Game
**  Copyright (C) 2009 Ingo Ruhnke <grumbel@gmail.com>
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

#ifndef HEADER_WINDSTILLE_SCENEGRAPH_SCENE_GRAPH_HPP
#define HEADER_WINDSTILLE_SCENEGRAPH_SCENE_GRAPH_HPP

#include <memory>
#include <vector>

class Drawable;
class DrawableGroup;
class GraphicsContext;
class Texture;

class SceneGraph
{
private:
  std::shared_ptr<DrawableGroup> m_drawables;

public:
  SceneGraph();

  std::shared_ptr<DrawableGroup> get_root() { return m_drawables; }

  void add_drawable(std::shared_ptr<Drawable> drawable);
  void remove_drawable(std::shared_ptr<Drawable> drawable);

  void render(GraphicsContext& gc, unsigned int mask);

  void clear();

private:
  SceneGraph(const SceneGraph&);
  SceneGraph& operator=(const SceneGraph&);
};

#endif

/* EOF */
