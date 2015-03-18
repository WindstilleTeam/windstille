#include <iostream>
#include "../src/math/matrix.hpp"

int main()
{
  Matrix m(0.0f, 0.1f, 0.2f, 0.3f,
           1.0f, 1.1f, 1.2f, 1.3f,
           2.0f, 2.1f, 2.2f, 2.3f,
           3.0f, 3.1f, 3.2f, 3.3f);

  std::cout << "Matrix: " << std::endl;
  std::cout << m << std::endl;

  std::cout << "Transpose: " << std::endl;
  std::cout << m.transpose() << std::endl;

  std::cout << "Transpose: " << std::endl;
  std::cout << m.inverse() << std::endl;

  return 0;
}

/* EOF */
