#include <stdlib.h>
#include <fstream>
#include <iostream>

#include "util/util.hpp"

int main(int argc, char* argv[])
{
  if (is_big_endian())
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
      uint32_t v_uint32 = read_uint32_t(in);
      uint16_t v_uint16 = read_uint16_t(in);
      float    v_float  = read_float(in);

       std::cout << "uint32: " << std::hex << v_uint32 << " - " << std::dec << v_uint32 << std::endl;
       std::cout << "uint16: " << std::hex << v_uint16 << " - " << std::dec << v_uint16 << std::endl;
      std::cout << "float:  " << v_float << std::endl;
    }
  }
  return 0;
}

/* EOF */
