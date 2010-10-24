#include <SDL_endian.h>
#include <stdlib.h>
#include <fstream>
#include <iostream>

#include "util/util.hpp"

int main(int argc, char* argv[])
{
  if (SDL_BYTEORDER == SDL_BIG_ENDIAN)
  {
    std::cout << "byte order is: big endian" << std::endl;
  }
  else
  {
    std::cout << "byte order is: little endian" << std::endl;
  }

  for(int i = 1; i < argc; ++i)
  {
    std::ifstream in(argv[i], std::ios::binary);
    if (!in)
    {
      std::cout << "couldn't open " << argv[i] << std::endl;
      exit(EXIT_FAILURE);
    }
    else
    {
      std::cout << "uint32: " << read_uint32_t(in) << std::endl;
      std::cout << "uint16: " << read_uint16_t(in) << std::endl;
      std::cout << "float:  " << read_float(in) << std::endl;
    }
  }
  return 0;
}

/* EOF */
