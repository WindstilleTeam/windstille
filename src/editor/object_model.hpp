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

#ifndef HEADER_WINDSTILLE_EDITOR_OBJECT_MODEL_HPP
#define HEADER_WINDSTILLE_EDITOR_OBJECT_MODEL_HPP

#include <boost/shared_ptr.hpp>
#include <string>

#include "display/surface.hpp"
#include "math/vector2f.hpp"

class ObjectModel;
class SceneContext;
typedef boost::shared_ptr<ObjectModel> ObjectModelHandle;

class ObjectModel
{
public:
  static ObjectModelHandle create(const std::string& name, const std::string& path, const Vector2f& pos);

private:
  std::string name;
  Vector2f     pos;
  Surface  surface;

public:
  ObjectModel(const std::string& name, const std::string& path, const Vector2f& pos);
  ~ObjectModel();
  
  std::string get_name() const { return name; }
  Vector2f get_pos() const { return pos; }
  
  void draw(SceneContext& sc);
  Rectf get_bounding_box() const;

private:
  ObjectModel(const ObjectModel&);
  ObjectModel& operator=(const ObjectModel&);
};

#endif

/* EOF */
