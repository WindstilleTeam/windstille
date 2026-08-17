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

#include "plugins/pnm.hpp"

#include <assert.h>
#include <stdexcept>

#include <geom/size.hpp>

#include "plugins/pnm_mem_reader.hpp"

namespace surf {
namespace pnm {

SoftwareSurface load_from_mem(std::span<uint8_t const> data)
{
  PNMMemReader pnm(data);

  PixelData<RGBPixel> dst(pnm.get_size());
  uint8_t const* src_pixels = pnm.get_pixel_data();
  RGBPixel* dst_pixels = dst.get_data();
  //std::cout << "MaxVal: " << pnm.get_maxval() << std::endl;
  assert(pnm.get_maxval() == 255);

  const int pixel_data_len = static_cast<int>((data.data() + data.size()) - src_pixels);

  if (pnm.get_magic() == "P6") // RGB
  {
    if (dst.get_width() * dst.get_height() * 3 > pixel_data_len)
    {
      throw std::runtime_error("PNM::load_from_mem(): premature end of pixel data");
    }

    for(int i = 0; i < dst.get_width() * dst.get_height(); ++i)
    {
      dst_pixels[i+0] = RGBPixel{
        src_pixels[3*i+0],
        src_pixels[3*i+1],
        src_pixels[3*i+2]
      };
    }
  }
  else if (pnm.get_magic() == "P5") // Grayscale
  {
    if (dst.get_width() * dst.get_height()  > pixel_data_len)
    {
      throw std::runtime_error("PNM::load_from_mem(): premature end of pixel data");
    }

    for(int i = 0; i < dst.get_width() * dst.get_height(); ++i)
    {
      dst_pixels[i+0] = RGBPixel{
        src_pixels[i],
        src_pixels[i],
        src_pixels[i]
      };
    }
  }
  else
  {
    throw std::runtime_error("PNM::load_from_mem(): Unhandled PNM format: '" + pnm.get_magic() + "'");
  }

  return SoftwareSurface(dst);
}

} // namespace pnm
} // namespace surf

/* EOF */
