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

#ifndef HEADER_WINDSTILLE_SPRITE2D_MANAGER_HPP
#define HEADER_WINDSTILLE_SPRITE2D_MANAGER_HPP

#include <filesystem>
#include <map>
#include <memory>

#include <wstdisplay/fwd.hpp>

namespace windstille {

class Sprite;
class SpriteData;

typedef std::shared_ptr<SpriteData> SpriteDataPtr;

class SpriteManager
{
public:
  SpriteManager(wstdisplay::SurfaceManager& surface_manager);
  ~SpriteManager();

  Sprite create(std::filesystem::path const& filename);
  SpriteDataPtr create_data(std::filesystem::path const& filename);

  /** Removes all cached Sprites that are no longer in use */
  void cleanup();

private:
  wstdisplay::SurfaceManager& m_surface_manager;
  typedef std::map<std::filesystem::path, SpriteDataPtr> Datas;
  Datas datas;
};

} // namespace windstille

#endif

/* EOF */
