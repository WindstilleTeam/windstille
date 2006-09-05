#ifndef __SPAWN_POINT_HPP__
#define __SPAWN_POINT_HPP__

#include <string>
#include "math/vector.hpp"
#include "lisp/lisp.hpp"

class SpawnPoint
{
public:
  SpawnPoint(const lisp::Lisp* lisp);

  std::string name;
  Vector pos;
};

#endif

