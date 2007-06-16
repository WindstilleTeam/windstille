/*  $Id$
**   __      __ __             ___        __   __ __   __
**  /  \    /  \__| ____    __| _/_______/  |_|__|  | |  |   ____
**  \   \/\/   /  |/    \  / __ |/  ___/\   __\  |  | |  | _/ __ \
**   \        /|  |   |  \/ /_/ |\___ \  |  | |  |  |_|  |_\  ___/
**    \__/\  / |__|___|  /\____ /____  > |__| |__|____/____/\___  >
**         \/          \/      \/    \/                         \/
**  Copyright (C) 2005 Ingo Ruhnke <grumbel@gmx.de>
**
**  This program is free software; you can redistribute it and/or
**  modify it under the terms of the GNU General Public License
**  as published by the Free Software Foundation; either version 2
**  of the License, or (at your option) any later version.
**
**  This program is distributed in the hope that it will be useful,
**  but WITHOUT ANY WARRANTY; without even the implied warranty of
**  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
**  GNU General Public License for more details.
** 
**  You should have received a copy of the GNU General Public License
**  along with this program; if not, write to the Free Software
**  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA
**  02111-1307, USA.
*/

#include <assert.h>
#include <iostream>
#include "math.hpp"
#include "response_curve.hpp"

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
      float bucket_size  = (i_max - i_min) / bucket_count;
      
      int bucket_index = int((v - i_min) / bucket_size);

      float t = ((v - i_min) - (bucket_index * bucket_size)) / bucket_size;
      
      return ((1.0f - t) * samples[bucket_index]) + (t * samples[bucket_index + 1]);
    }
}

#ifdef __TEST__
#include <iostream>

int main(int argc, char** argv)
{
  float samples[] = { 0.0f, 0.25f, 0.75f, 1.0f };
  std::vector<float> samples_vec(samples, samples+4);

  std::cout << "Samples: " << samples_vec.size() << std::endl;

  ResponseCurve curve(0.0f, 1.0f, samples_vec);

  std::cout << curve.get(atof(argv[1])) << std::endl;

  return 0;
}
#endif

/* EOF */
