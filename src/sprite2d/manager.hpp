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

#include <wstdisplay/fwd.hpp>
#include <wstsprite/sprite_manager.hpp>

namespace windstille {

class Sprite;

/** Loads 2D sprites, .sprite files as well as plain images. A missing
    .sprite file falls back to a .png of the same name. */
class SpriteManager
{
public:
  explicit SpriteManager(wstdisplay::SurfaceManager& surface_manager);
  ~SpriteManager();

  Sprite create(std::filesystem::path const& filename);

  /** Throws if \a filename can't be loaded */
  wstsprite::SpriteId load(std::filesystem::path const& filename);

  wstsprite::SpriteData const& get(wstsprite::SpriteId id) const { return m_sprites.get(id); }
  wstsprite::SpriteData const* find(wstsprite::SpriteId id) const { return m_sprites.find(id); }

private:
  wstsprite::SpriteManager m_sprites;

public:
  SpriteManager(SpriteManager const&) = delete;
  SpriteManager& operator=(SpriteManager const&) = delete;
};

} // namespace windstille

#endif

/* EOF */
