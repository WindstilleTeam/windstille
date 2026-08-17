// biio - Binary file I/O
// Copyright (C) 2020 Ingo Ruhnke <grumbel@gmail.com>
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

#ifndef HEADER_BIIO_READER_HPP
#define HEADER_BIIO_READER_HPP

#include <array>
#include <algorithm>
#include <filesystem>
#include <istream>
#include <memory>

#include "bit_cast.hpp"

namespace biio {

class Reader
{
public:
  static Reader from_file(std::filesystem::path const& filename);
  static Reader from_stream(std::unique_ptr<std::istream> stream);
  static Reader from_stream(std::istream& stream);

private:
  Reader(std::unique_ptr<std::istream> stream);
  Reader(std::istream& stream);

public:
  template<typename T>
  T read() {
    static_assert(std::is_trivially_constructible_v<T>,
                  "type is not trivially constructible");
    static_assert(!std::is_arithmetic_v<T>,
                  "explicit endianess request required for integer and floating point");

    std::array<char, sizeof(T)> buffer = {};
    if (!m_stream.read(buffer.data(), buffer.size())) {
      throw std::runtime_error("biio::read() failed");
    }
    return biio::bit_cast<T>(buffer);
  }

  /** Read a value in little-endian order and convert it to native */
  template<typename T>
  T read_le() {
    static_assert(std::is_arithmetic_v<T>,
                  "type must be integer or floating point");

    std::array<char, sizeof(T)> buffer = {};
    if (!m_stream.read(buffer.data(), buffer.size())) {
      throw std::runtime_error("biio::read_le() failed");
    }

    if constexpr (std::endian::native != std::endian::little) {
      std::reverse(buffer.begin(), buffer.end());
    }

    return biio::bit_cast<T>(buffer);
  }

  /** Read a value in big-endian order and convert it to native */
  template<typename T>
  T read_be() {
    static_assert(std::is_arithmetic_v<T>,
                  "type must be integer or floating point");

    std::array<char, sizeof(T)> buffer = {};
    if (!m_stream.read(buffer.data(), buffer.size())) {
      throw std::runtime_error("biio::read_be() failed");
    }

    if constexpr (std::endian::native != std::endian::big) {
      std::reverse(buffer.begin(), buffer.end());
    }

    return biio::bit_cast<T>(buffer);
  }

  std::string read_string(size_t len);
  std::string read_string0(size_t maxlen);

private:
  std::istream& m_stream;
  std::unique_ptr<std::istream> m_stream_owner;

private:
  Reader(const Reader&) = delete;
  Reader& operator=(const Reader&) = delete;
};

} // namespace biio

#endif

/* EOF */
