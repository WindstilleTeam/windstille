// surf - Software surface library
// Copyright (C) 2026 Ingo Ruhnke <grumbel@gmail.com>
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

#ifndef HEADER_SURF_PLUGINS_STB_IMAGE_SAVER_HPP
#define HEADER_SURF_PLUGINS_STB_IMAGE_SAVER_HPP

#include <filesystem>

#include <surf/software_surface.hpp>

namespace surf {
namespace stb_image_saver {

/** Save with stb_image_write, for builds without libpng and libjpeg */
void save_png(SoftwareSurface const& surface, std::filesystem::path const& filename);
void save_jpeg(SoftwareSurface const& surface, std::filesystem::path const& filename, int quality);

} // namespace stb_image_saver
} // namespace surf

#endif

/* EOF */
