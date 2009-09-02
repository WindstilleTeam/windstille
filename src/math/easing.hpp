/*
**  Copyright © 2001 Robert Penner
**  All rights reserved.
**  
**  Redistribution and use in source and binary forms, with or without
**  modification, are permitted provided that the following conditions
**  are met:
**  
**  * Redistributions of source code must retain the above copyright
**    notice, this list of conditions and the following disclaimer.
**  
**  * Redistributions in binary form must reproduce the above
**    copyright notice, this list of conditions and the following
**    disclaimer in the documentation and/or other materials provided
**    with the distribution.
**
**  * Neither the name of the author nor the names of contributors may
**    be used to endorse or promote products derived from this
**    software without specific prior written permission.
**
**  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND
**  CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES,
**  INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
**  MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
**  DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS
**  BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
**  EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED
**  TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
**  DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
**  ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR
**  TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF
**  THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
**  SUCH DAMAGE.
*/

//  Based on http://www.robertpenner.com/easing/
//  Ported to C++ by Ingo Ruhnke <grumbel@gmx.de>

#ifndef HEADER_WINDSTILLE_MATH_EASING_HPP
#define HEADER_WINDSTILLE_MATH_EASING_HPP

#include <math.h>

#include "math/math.hpp"

namespace math {
namespace easing {

namespace back {

inline float ease_in (float t, float b, float c, float d, float s = 1.70158f)
{
  t /= d;
  return c*t*t*((s+1.0f)*t - s) + b;
}

inline float ease_out (float t, float b, float c, float d, float s = 1.70158f)
{
  t=t/d-1;
  return c*(t*t*((s+1)*t + s) + 1) + b;
}

inline float ease_in_out (float t, float b, float c, float d, float s = 1.70158f)
{
  t /= d/2;
  s*=1.525f;
  if (t < 1) return c/2*(t*t*((s+1)*t - s)) + b;
  s*=1.525f;
  t-=2;
  return c/2*(t*t*((s+1)*t + s) + 2) + b;
}

} // namespace back

namespace bounce {

inline float ease_out (float t, float b, float c, float d) 
{
  t  /=  d;
  if (t < (1/2.75f)) 
  {
    return c*(7.5625f*t*t) + b;
  }
  else if (t < (2/2.75f)) 
  {
    t-=1.5f/2.75f;
    return c*(7.5625f*t*t + .75f) + b;
  }
  else if (t < (2.5f/2.75f)) 
  {
    t-=(2.25f/2.75f);
    return c*(7.5625f*t*t + .9375f) + b;
  } 
  else 
  {
    t-=(2.625f/2.75f);
    return c*(7.5625f*t*t + .984375f) + b;
  }
}

inline float ease_in (float t, float b, float c, float d) 
{
  return c - ease_out (d-t, 0, c, d) + b;
}

inline float ease_in_out (float t, float b, float c, float d) 
{
  if (t < d/2) 
  {
    return ease_in (t*2, 0, c, d) * .5f + b;
  }
  else
  {
    return ease_out (t*2-d, 0, c, d) * .5f + c*.5f + b;
  }
}

} // namespace bounce

namespace circ {

inline float ease_in (float t, float b, float c, float d) 
{
  t  /=  d;
  return -c * (sqrtf(1 - t*t) - 1) + b;
}

inline float ease_out (float t, float b, float c, float d) 
{
  t=t/d-1;
  return c * sqrtf(1 - t*t) + b;
}

inline float ease_in_out (float t, float b, float c, float d) 
{
  t /= d/2;
  if (t < 1) return -c/2 * (sqrtf(1 - t*t) - 1) + b;
  t-=2;
  return c/2 * (sqrtf(1 - t*t) + 1) + b;
}

} // namespace circ {

namespace cubic {

inline float ease_in (float t, float b, float c, float d) 
{
  t /= d;
  return c*t*t*t + b;
}

inline float ease_out (float t, float b, float c, float d) 
{
  t=t/d-1;
  return c*(t*t*t + 1) + b;
}

inline float ease_in_out (float t, float b, float c, float d) 
{
  t /= d/2;
  if (t < 1) return c/2*t*t*t + b;
  t-=2;
  return c/2*(t*t*t + 2) + b;
}

} // namespace cubic

namespace elastic {

inline float ease_in (float t, float b, float c, float d, float a, float p) 
{
  if (t==0) return b; 
  t /= d;
  if (t==1) return b+c;
  if (!p) p=d*0.3f;

  float s;

  if (!a || a < fabsf(c))
  {
    a=c; s=p/4; 
  }
  else 
  {
    s = p/(2*math::pi) * asinf (c/a);
  }

  t-=1;
  return -(a*powf(2,10*t) * sinf( (t*d-s)*(2*math::pi)/p)) + b;
}

inline float ease_out (float t, float b, float c, float d, float a, float p) 
{
  if (t==0) return b;  
  t /= d;
  if (t==1) return b+c;  if (!p) p=d*.3f;
  float s;
  if (!a || a < fabsf(c)) { a=c; s=p/4; }
  else s = p/(2*math::pi) * asinf (c/a);
  return (a*powf(2,-10*t) * sinf( (t*d-s)*(2*math::pi)/p ) + c + b);
}

inline float ease_in_out (float t, float b, float c, float d, float a, float p) 
{
  t/=d/2;
  if (t==0) return b;  if (t==2) return b+c;  if (!p) p=d*(.3f*1.5f);
  float s;
  if (!a || a < fabsf(c)) { a=c; s=p/4; }
  else s = p/(2*math::pi) * asinf(c/a);
  t-=1;
  if (t < 1) return -.5f*(a*powf(2,10*t) * sinf( (t*d-s)*(2*math::pi)/p )) + b;
  t-=1;
  return a*powf(2,-10*t) * sinf( (t*d-s)*(2*math::pi)/p )*.5f + c + b;
}

} // namespace elastic

namespace expo {
 
inline float ease_in (float t, float b, float c, float d)
{
  return (t==0) ? b : c * powf(2, 10 * (t/d - 1)) + b;
}

inline float ease_out (float t, float b, float c, float d)
{
  return (t==d) ? b+c : c * (-powf(2, -10 * t/d) + 1) + b;
}

inline float ease_in_out (float t, float b, float c, float d)
{
  if (t==0) return b;
  if (t==d) return b+c;
  t /= d/2;
  if (t < 1) return c/2 * powf(2, 10 * (t - 1)) + b;
  return c/2 * (-powf(2, -10 * --t) + 2) + b;
}

} // namespace expo

namespace linear {

inline float easeNone (float t, float b, float c, float d) 
{
  return c*t/d + b;
}

inline float ease_in (float t, float b, float c, float d) 
{
  return c*t/d + b;
}

inline float ease_out (float t, float b, float c, float d) 
{
  return c*t/d + b;
}

inline float ease_in_out (float t, float b, float c, float d) 
{
  return c*t/d + b;
}

} // namespace linear

namespace quad {

inline float ease_in (float t, float b, float c, float d) 
{
  t/=d;
  return c*t*t + b;
}

inline float ease_out (float t, float b, float c, float d) 
{
  t/=d;
  return -c *t*(t-2) + b;
}

inline float ease_in_out (float t, float b, float c, float d) 
{
  t/=d/2;
  if (t < 1) return c/2*t*t + b;
  --t;
  return -c/2 * (t*(t-2) - 1) + b;
}

} // namespace quad

namespace quart {

inline float ease_in (float t, float b, float c, float d) 
{
  t /= d;
  return c*t*t*t*t + b;
}

inline float ease_out (float t, float b, float c, float d) 
{
  t = t/d-1;
  return -c * (t*t*t*t - 1) + b;
}

inline float ease_in_out (float t, float b, float c, float d)
{
  t /= d/2;
  if (t < 1) return c/2*t*t*t*t + b;
  t -= 2;
  return -c/2 * (t*t*t*t - 2) + b;
}

} // namespace quart

namespace quint {

inline float ease_in (float t, float b, float c, float d)
{
  t /= d;
  return c*t*t*t*t*t + b;
}

inline float ease_out (float t, float b, float c, float d)
{
  t = t/d-1;
  return c*(t*t*t*t*t + 1) + b;
}

inline float ease_in_out (float t, float b, float c, float d)
{
  t /= d/2;
  if (t < 1) return c/2*t*t*t*t*t + b;
  t -= 2;
  return c/2*(t*t*t*t*t + 2) + b;
}

} // namespace quint

namespace sine {

inline float ease_in (float t, float b, float c, float d) 
{
  return -c * cosf(t/d * (math::pi/2)) + c + b;
}

inline float ease_out (float t, float b, float c, float d) 
{
  return c * sinf(t/d * (math::pi/2)) + b;
}

inline float ease_in_out (float t, float b, float c, float d) 
{
  return -c/2 * (cosf(math::pi*t/d) - 1) + b;
}

} // namespace sine

} // namespace easing
} // namespace math

#endif

/* EOF */
