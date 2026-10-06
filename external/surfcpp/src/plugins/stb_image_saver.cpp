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

#include "plugins/stb_image_saver.hpp"

#include <stdexcept>
#include <string>

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

namespace surf {
namespace stb_image_saver {

namespace {

/** stb_image_write takes tightly packed RGBA rows */
SoftwareSurface to_rgba(SoftwareSurface const& surface)
{
  return surface.get_format() == PixelFormat::RGBA8 ? surface : convert(surface, PixelFormat::RGBA8);
}

} // namespace

void save_png(SoftwareSurface const& surface, std::filesystem::path const& filename)
{
  SoftwareSurface const rgba = to_rgba(surface);
  if (!stbi_write_png(filename.string().c_str(), rgba.get_width(), rgba.get_height(), 4,
                      rgba.get_data(), rgba.get_pitch()))
  {
    throw std::runtime_error("stb_image_write: failed to save " + filename.string());
  }
}

void save_jpeg(SoftwareSurface const& surface, std::filesystem::path const& filename, int quality)
{
  SoftwareSurface const rgba = to_rgba(surface);
  if (rgba.get_pitch() != rgba.get_width() * 4) {
    throw std::runtime_error("stb_image_write: JPEG needs tightly packed rows");
  }
  if (!stbi_write_jpg(filename.string().c_str(), rgba.get_width(), rgba.get_height(), 4,
                      rgba.get_data(), quality))
  {
    throw std::runtime_error("stb_image_write: failed to save " + filename.string());
  }
}

} // namespace stb_image_saver
} // namespace surf

/* EOF */
