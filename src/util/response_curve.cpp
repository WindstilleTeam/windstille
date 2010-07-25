/*
**  Windstille - A Sci-Fi Action-Adventure Game
**  Copyright (C) 2005 Ingo Ruhnke <grumbel@gmx.de>
**
**  This program is free software: you can redistribute it and/or modify
**  it under the terms of the GNU General Public License as published by
**  the Free Software Foundation, either version 3 of the License, or
**  (at your option) any later version.
**  
**  This program is distributed in the hope that it will be useful,
**  but WITHOUT ANY WARRANTY; without even the implied warranty of
**  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
**  GNU General Public License for more details.
**  
**  You should have received a copy of the GNU General Public License
**  along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "math/math.hpp"
#include "util/response_curve.hpp"

ResponseCurve::ResponseCurve(float i_min_, float i_max_, const std::vector<float>& samples_)
  : i_min(i_min_),
    i_max(i_max_),
    samples(samples_)
{
  assert(i_max > i_min);
  assert(!samples.empty());
}

ResponseCurve::~ResponseCurve()
{
}

float
ResponseCurve::get(float v)
{
  if (samples.size() == 1)
  {
    return samples[0];
  }
  else
  {
    v = math::mid(i_min, v, i_max);

    int   bucket_count = samples.size() - 1;
    float bucket_size  = (i_max - i_min) / static_cast<float>(bucket_count);
      
    int bucket_index = int((v - i_min) / bucket_size);

    float t = ((v - i_min) - (static_cast<float>(bucket_index) * bucket_size)) / bucket_size;
      
    return ((1.0f - t) * samples[bucket_index]) + (t * samples[bucket_index + 1]);
  }
}

#ifdef __TEST__
#include <iostream>
#include <stdlib.h>

int main(int argc, char** argv)
{
  if (argc != 2)
  {
    std::cout << "Usage: " << argv[0] << " FLOAT" << std::endl;
  }
  else
  {
    float samples[] = { 0.0f, 0.25f, 0.75f, 1.0f };
    std::vector<float> samples_vec(samples, samples+4);

    std::cout << "Samples: " << samples_vec.size() << std::endl;

    ResponseCurve curve(0.0f, 1.0f, samples_vec);

    std::cout << curve.get(static_cast<float>(atof(argv[1]))) << std::endl;
  }
  return 0;
}
#endif

/* EOF */
