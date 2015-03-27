#include <iostream>
#include <stdlib.h>
#include <vector>

#include "util/response_curve.hpp"

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

/* EOF */

