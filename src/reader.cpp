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

#include "reader.hpp"

#include <fstream>
#include <sstream>
#include <string.h>

namespace biio {

Reader
Reader::from_file(std::filesystem::path const& filename)
{
  std::ifstream stream(filename, std::ios::binary);
  if (!stream) {
    std::ostringstream oss;
    oss << filename << ": failed to open: " << strerror(errno);
    throw std::runtime_error(oss.str());
  }

  return Reader(std::make_unique<std::ifstream>(std::move(stream)));
}

Reader
Reader::from_stream(std::unique_ptr<std::istream> stream)
{
  return Reader(std::move(stream));
}

Reader
Reader::from_stream(std::istream& stream)
{
  return Reader(stream);
}

Reader::Reader(std::unique_ptr<std::istream> stream) :
  m_stream(*stream),
  m_stream_owner(std::move(stream))
{
}

Reader::Reader(std::istream& stream) :
  m_stream(stream),
  m_stream_owner()
{
}

std::string
Reader::read_string(size_t len)
{
  std::string value(len, ' ');
  if (!m_stream.read(value.data(), len)) {
    throw std::runtime_error("biio::read_string() failed");
  }
  return value;
}

std::string
Reader::read_string0(size_t maxlen)
{
  std::string value = read_string(maxlen);
  std::string::size_type p = value.find('\0');
  if (p != std::string::npos) {
    value.resize(p);
  }
  return value;
}

} // namespace biio

/* EOF */
