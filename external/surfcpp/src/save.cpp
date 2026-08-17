// surf - Software surface library
// Copyright (C) 2008-2020 Ingo Ruhnke <grumbel@gmail.com>
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

#include <stdexcept>

#include "pixel_data.hpp"
#include "plugins/jpeg.hpp"
#include "plugins/png.hpp"
#include "save.hpp"

namespace surf {

void save(SoftwareSurface const& surface, std::filesystem::path const& path, std::string_view format)
{
  if (format == "auto") {
    if (path.extension() == ".jpg" || path.extension() == ".JPG") {
      surf::jpeg::save(surface, path, 70);
    } else if (path.extension() == ".png" || path.extension() == ".PNG") {
      surf::png::save(surface, path);
    } else {
      throw std::invalid_argument("unknown file extension");
    }
  } else if (format == "png") {
    surf::png::save(surface, path);
  } else if (format == "jpeg") {
    surf::jpeg::save(surface, path, 70);
  } else {
    throw std::runtime_error("unsupported format");
  }
}

} // namespace surf

/* EOF */
