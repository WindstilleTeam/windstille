#include <gtest/gtest.h>

#include <surf/hsv.hpp>

using namespace surf;

TEST(HSVTest, hsv_from_color)
{
  HSVColor hsv;

  hsv = hsv_from_color(Color(1.0f, 0.0f, 0.0f));
  EXPECT_FLOAT_EQ(hsv.hue, 0.0f);
  EXPECT_FLOAT_EQ(hsv.value, 1.0f);
  EXPECT_FLOAT_EQ(hsv.saturation, 1.0f);

  hsv = hsv_from_color(Color(0.5f, 0.0f, 0.0f));
  EXPECT_FLOAT_EQ(hsv.hue, 0.0f);
  EXPECT_FLOAT_EQ(hsv.value, 0.5f);
  EXPECT_FLOAT_EQ(hsv.saturation, 1.0f);

  hsv = hsv_from_color(Color(1.0f, 0.5f, 0.5f));
  EXPECT_FLOAT_EQ(hsv.hue, 0.0f);
  EXPECT_FLOAT_EQ(hsv.value, 1.0f);
  EXPECT_FLOAT_EQ(hsv.saturation, 0.5f);

  hsv = hsv_from_color(Color(0.0f, 1.0f, 0.0f));
  EXPECT_FLOAT_EQ(hsv.hue, 0.33333334f);
  EXPECT_FLOAT_EQ(hsv.value, 1.0f);
  EXPECT_FLOAT_EQ(hsv.saturation, 1.0f);
}

TEST(HSVTest, color_from_hue)
{
  Color color;

  color = color_from_hue(0.0f);
  EXPECT_FLOAT_EQ(color.r, 1.0f);
  EXPECT_FLOAT_EQ(color.g, 0.0f);
  EXPECT_FLOAT_EQ(color.b, 0.0f);
}


TEST(HSVTest, color_from_hsv)
{
  Color color;

  color = color_from_hsv(HSVColor{0.0f, 1.0f, 1.0f});
  EXPECT_FLOAT_EQ(color.r, 1.0f);
  EXPECT_FLOAT_EQ(color.g, 0.0f);
  EXPECT_FLOAT_EQ(color.b, 0.0f);

  color = color_from_hsv(HSVColor{0.0f, 1.0f, 0.5f});
  EXPECT_FLOAT_EQ(color.r, 0.5f);
  EXPECT_FLOAT_EQ(color.g, 0.0f);
  EXPECT_FLOAT_EQ(color.b, 0.0f);
}

/* EOF */
