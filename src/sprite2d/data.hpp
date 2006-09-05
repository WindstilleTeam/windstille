#ifndef __SPRITE_DATA_HPP__
#define __SPRITE_DATA_HPP__

#include <string>
#include <vector>
#include <GL/glew.h>
#include <GL/gl.h>
#include "math/vector.hpp"
#include "display/surface.hpp"
#include "lisp/lisp.hpp"
#include "ref.hpp"

namespace sprite2d
{
struct Action;

class Data
{
public:
  Data(const std::string& filename);
  virtual ~Data();

  typedef std::vector<Action*> Actions;
  Actions actions;

private:
  void parse(const std::string& dir, const lisp::Lisp* lisp);
  
  Action* parse_action(const std::string& dir, const lisp::Lisp* lisp);
  void parse_images(Action* action, const std::string& dir, 
                    const lisp::Lisp* lisp);
  void parse_image_grid(Action* action, const std::string& dir,
                        const lisp::Lisp* lisp); 
};

struct Action
{
  typedef std::vector<Surface> Surfaces;

  std::string name;
  float       speed;
  Vector      offset;
  float       scale;
  Surfaces    surfaces;
};

}

#endif

