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

#ifndef HEADER_WINDSTILLE_EDITOR_DECAL_OBJECT_MODEL_HPP
#define HEADER_WINDSTILLE_EDITOR_DECAL_OBJECT_MODEL_HPP

#include "display/surface.hpp"
#include "object_model.hpp"

class DecalObjectModel : public ObjectModel
{
public:
  enum MapType { COLORMAP, LIGHTMAP, HIGHLIGHTMAP };

  static ObjectModelHandle create(const std::string& name, const Vector2f& pos, const std::string& path, MapType type);

private:
  Surface surface;
  MapType type;

public:
  DecalObjectModel(const std::string& name, const Vector2f& rel_pos, 
                   const std::string& path_, MapType type_);
  ~DecalObjectModel();

  void draw(SceneContext& sc);
  Rectf get_bounding_box() const;
  ObjectModelHandle clone() const;
};

#endif

/* EOF */
