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

#ifndef HEADER_SURF_PIXEL_DATA_HPP
#define HEADER_SURF_PIXEL_DATA_HPP

#include "pixel_view.hpp"

namespace surf {

template<typename Pixel>
class PixelData : public PixelView<Pixel>
{
public:
  PixelData() :
    PixelView<Pixel>(),
    m_pixels_ownership()
  {}

  PixelData(PixelData<Pixel> const& other) :
    PixelView<Pixel>(other),
    m_pixels_ownership(other.m_pixels_ownership)
  {
    this->m_pixels = m_pixels_ownership.data();
  }

  PixelData<Pixel>& operator=(PixelData<Pixel> const& other)
  {
    if (this != &other) {
      PixelView<Pixel>::operator=(other);
      m_pixels_ownership = other.m_pixels_ownership;
      this->m_pixels = m_pixels_ownership.data();
    }
    return *this;
  }

  PixelData(PixelData<Pixel>&& other) noexcept :
    PixelView<Pixel>(other),
    m_pixels_ownership(std::move(other.m_pixels_ownership))
  {
    this->m_pixels = m_pixels_ownership.data();
  }

  PixelData<Pixel>& operator=(PixelData<Pixel>&& other) noexcept
  {
    if (this != &other) {
      PixelView<Pixel>::operator=(other);
      m_pixels_ownership = std::move(other.m_pixels_ownership);
      this->m_pixels = m_pixels_ownership.data();
    }
    return *this;
  }

  PixelData(PixelView<Pixel> const& view) :
    PixelView<Pixel>(view),
    m_pixels_ownership(geom::area(this->m_size))
  {
    this->m_pixels = m_pixels_ownership.data();
    for (int y = 0; y < this->m_size.height(); ++y) {
      std::copy_n(view.get_row(y), this->m_size.width(), this->get_row(y));
    }
  }

  PixelData(geom::isize const& size, Pixel const& pixel = {}) :
    PixelView<Pixel>(size, nullptr),
    m_pixels_ownership(geom::area(size), pixel)
  {
    this->m_pixels = m_pixels_ownership.data();
  }

  PixelData(geom::isize const& size, std::vector<Pixel> pixels) :
    PixelView<Pixel>(size, nullptr),
    m_pixels_ownership(std::move(pixels))
  {
    this->m_pixels = m_pixels_ownership.data();
  }

  PixelData(geom::isize const& size, std::vector<Pixel> pixels, int row_length) :
    PixelView<Pixel>(size, nullptr, row_length),
    m_pixels_ownership(std::move(pixels))
  {
    this->m_pixels = m_pixels_ownership.data();
  }

private:
  std::vector<Pixel> m_pixels_ownership;
};

} // namespace surf

#endif

/* EOF */
