#include "spawnpoint.hpp"

#include <iostream>
#include <stdexcept>
#include "lisp/properties.hpp"
#include "lisp_getters.hpp"

SpawnPoint::SpawnPoint(const lisp::Lisp* lisp)
{
  using namespace lisp;

  Properties props(lisp);
  props.get("name", name);
  props.get("pos", pos);
  props.print_unused_warnings("spawnpoint");

  if(name == "")
    throw std::runtime_error("No name specified for spawnpoint");
}
