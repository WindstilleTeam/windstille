#include <gtest/gtest.h>

#include <surf/convert.hpp>
#include <surf/pixel.hpp>

using namespace surf;

TEST(ConvertTest, convert_value)
{
  EXPECT_EQ(RGB32Pixel::max(), 4294967295);
  EXPECT_EQ((convert_value<Color, RGB32Pixel>(1.0f)), 4294967295);
}

TEST(ConvertTest, convert_rgb)
{
  RGB32Pixel rgb32 = convert<Color, RGB32Pixel>(Color(1.0f, 1.0f, 1.0f));
  EXPECT_EQ(rgb32.r, 4294967295);
  EXPECT_EQ(rgb32.g, 4294967295);
  EXPECT_EQ(rgb32.b, 4294967295);

  RGB8Pixel rgb8 = convert<RGB32Pixel, RGB8Pixel>(rgb32);
  EXPECT_EQ(rgb8.r, 255);
  EXPECT_EQ(rgb8.g, 255);
  EXPECT_EQ(rgb8.b, 255);
}

TEST(ConvertTest, convert_rgba)
{
  RGBA32Pixel rgb32 = convert<Color, RGBA32Pixel>(Color(1.0f, 1.0f, 1.0f));
  EXPECT_EQ(rgb32.r, 4294967295);
  EXPECT_EQ(rgb32.g, 4294967295);
  EXPECT_EQ(rgb32.b, 4294967295);
  EXPECT_EQ(rgb32.a, 4294967295);

  RGBA8Pixel rgb8 = convert<RGBA32Pixel, RGBA8Pixel>(rgb32);
  EXPECT_EQ(rgb8.r, 255);
  EXPECT_EQ(rgb8.g, 255);
  EXPECT_EQ(rgb8.b, 255);
  EXPECT_EQ(rgb8.a, 255);
}

/* EOF */

