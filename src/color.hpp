#ifndef __COLOR_HPP__
#define __COLOR_HPP__

class Color
{
public:
  Color()
    : r(0), g(0), b(0), a(1.0)
  { }
  Color(float r, float g, float b, float a = 1.0)
    : r(r), g(g), b(b), a(a)
  { }

  float r, g, b, a;
};

#endif

