#include <iostream>

#include "math/random.hpp"

int main()
{
  Random random(5489UL);
  for(int i = 0; i < 1000000; ++i)
  {
    std::cout << random.drand(0, 5) << std::endl;
  }
  return 0;
}

/* EOF */
