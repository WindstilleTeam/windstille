/*
**  Windstille - A Sci-Fi Action-Adventure Game
**  Copyright (C) 2005 Matthias Braun <matze@braunis.de>
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

#ifndef HEADER_WINDSTILLE_DISPLAY_SURFACE_MANAGER_HPP
#define HEADER_WINDSTILLE_DISPLAY_SURFACE_MANAGER_HPP

#include <memory>
#include <string>
#include <vector>
#include <map>

#include "display/texture.hpp"
#include "display/surface.hpp"

class TexturePacker;

/** This class keeps a list of loaded surfaces and manages loading new ones */
class SurfaceManager final
{
private:
  std::unique_ptr<TexturePacker> texture_packer;

  typedef std::map<std::filesystem::path, SurfacePtr> Surfaces;
  Surfaces surfaces;

public:
  SurfaceManager();
  ~SurfaceManager();

  /** returns a surface containing the image specified with filename */
  SurfacePtr get(std::filesystem::path const& filename);

  /**
   * Loads an image and splits it into several Surfaces sized width and height.
   * The created surfaces will be added to the surfaces vector.
   */
  void load_grid(std::filesystem::path const& filename,
                 std::vector<SurfacePtr>& surfaces, int width, int height);

  TexturePtr create_texture(SoftwareSurface const& image,
                            float* maxu, float* maxv);

  /** Removes all cached Sprites that are no longer in use */
  void cleanup();

  void save_all_as_png() const;
};

#endif

/* EOF */
