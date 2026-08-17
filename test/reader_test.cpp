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

#include <gtest/gtest.h>

#include <sstream>

#include <biio/reader.hpp>

using namespace biio;

TEST(ReaderTest, read)
{
  std::istringstream stream(std::string("\x00\x00\x00\x90", 4));
  auto binreader = Reader::from_stream(stream);
  binreader.read<std::array<char, 4>>();
}

TEST(ReaderTest, read_fail)
{
  std::istringstream stream(std::string("\x00\x00\x00\x90", 4));
  auto binreader = Reader::from_stream(stream);
  EXPECT_ANY_THROW((binreader.read<std::array<char,8> >()));
}

TEST(ReaderTest, read_le)
{
  std::istringstream stream(std::string("\x90\x00\x00\x00", 4));
  auto binreader = Reader::from_stream(stream);
  EXPECT_EQ(0x90, binreader.read_le<uint32_t>());
}

TEST(ReaderTest, read_le_fail)
{
  std::istringstream stream(std::string("\x90\x00", 2));
  auto binreader = Reader::from_stream(stream);
  EXPECT_ANY_THROW((binreader.read_le<uint32_t>()));
}

TEST(ReaderTest, read_be)
{
  std::istringstream stream(std::string("\x00\x00\x00\x90", 4));
  auto binreader = Reader::from_stream(stream);
  EXPECT_EQ(0x90, binreader.read_be<uint32_t>());
}

TEST(ReaderTest, read_be_fail)
{
  std::istringstream stream(std::string("\x00\x00", 2));
  auto binreader = Reader::from_stream(stream);
  EXPECT_ANY_THROW((binreader.read_be<uint32_t>()));
}

TEST(ReaderTest, read_string)
{
  std::istringstream stream("Hello World");
  auto binreader = Reader::from_stream(stream);
  EXPECT_EQ("Hello", binreader.read_string(5));
  EXPECT_EQ(" ", binreader.read_string(1));
  EXPECT_EQ("World", binreader.read_string(5));
}

TEST(ReaderTest, read_string0)
{
  std::istringstream stream(std::string("Hello\0\0\0World\0\0\0", 16));
  auto binreader = Reader::from_stream(stream);
  EXPECT_EQ("Hello", binreader.read_string0(8));
  EXPECT_EQ("World", binreader.read_string0(8));
}

/* EOF */
