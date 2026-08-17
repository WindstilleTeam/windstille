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

#ifndef HEADER_WINDSTILLE_DISPLAY_SURFACE_MANAGER_HPP
#define HEADER_WINDSTILLE_DISPLAY_SURFACE_MANAGER_HPP

#include <map>
#include <memory>
#include <string>
#include <vector>

#include "texture.hpp"
#include "surface.hpp"

namespace wstdisplay {

class TexturePacker;

/** This class keeps a list of loaded surfaces and manages loading new ones */
class SurfaceManager final
{
public:
  SurfaceManager();
  ~SurfaceManager();

  /** returns a surface containing the image specified with filename */
  SurfacePtr get(std::filesystem::path const& filename);

  /** Loads an image and splits it into several Surfaces sized width and height.
      The created surfaces will be added to the surfaces vector. */
  void load_grid(std::filesystem::path const& filename,
                 std::vector<SurfacePtr>& surfaces, geom::isize const& size);

  TexturePtr create_texture(SoftwareSurface const& image,
                            float* maxu, float* maxv);

  /** Removes all cached Sprites that are no longer in use */
  void cleanup();

  void save_all_as_png() const;

private:
  std::unique_ptr<TexturePacker> m_texture_packer;
  std::map<std::filesystem::path, SurfacePtr> m_surfaces;
};

} // namespace wstdisplay

#endif

/* EOF */
