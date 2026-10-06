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

#include "sprite2d/manager.hpp"

#include <stdexcept>

#include "sprite2d/sprite.hpp"

namespace windstille {

SpriteManager::SpriteManager(wstdisplay::SurfaceManager& surface_manager) :
  m_sprites(surface_manager)
{
}

SpriteManager::~SpriteManager()
{
}

wstsprite::SpriteId
SpriteManager::load(std::filesystem::path const& filename)
{
  if (filename.extension() == ".sprite" && !std::filesystem::exists(filename))
  {
    std::filesystem::path pngfile = filename;
    pngfile.replace_extension(".png");
    if (!std::filesystem::exists(pngfile)) {
      throw std::runtime_error("couldn't find " + filename.string() + " or " + pngfile.string());
    }
    return m_sprites.load(pngfile);
  }

  return m_sprites.load(filename);
}

Sprite
SpriteManager::create(std::filesystem::path const& filename)
{
  return Sprite(filename, *this);
}

} // namespace windstille

/* EOF */
