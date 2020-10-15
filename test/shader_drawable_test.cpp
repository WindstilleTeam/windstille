#include <iostream>
#include <sstream>
#include <stdexcept>
#include <SDL.h>

#include "display/opengl_window.hpp"
#include "display/surface.hpp"
#include "display/surface_manager.hpp"
#include "display/texture_manager.hpp"
#include "scenegraph/drawable_group.hpp"
#include "scenegraph/surface_drawable.hpp"
#include "scenegraph/scissor_drawable.hpp"
#include "scenegraph/stencil_drawable.hpp"
#include "scenegraph/shader_drawable.hpp"
#include "display/shader_object.hpp"
#include "util/pathname.hpp"
#include "util/system.hpp"

namespace {

int shader_main(int argc, char* argv[])
{
  Pathname::set_datadir("data/"); //System::find_default_datadir());
  Pathname::set_userdir(System::find_default_userdir());

#ifdef DEBUG
  // I wanna have usefull backtraces in debug mode
  Uint32 flags = SDL_INIT_VIDEO | SDL_INIT_JOYSTICK | SDL_INIT_NOPARACHUTE;
#else
  Uint32 flags = SDL_INIT_VIDEO | SDL_INIT_JOYSTICK;
#endif

  if (SDL_Init(flags) < 0)
  {
    std::stringstream msg;
    msg << "Couldn't initialize SDL: " << SDL_GetError();
    throw std::runtime_error(msg.str());
  }
  else
  {
    atexit(SDL_Quit);
  }

  OpenGLWindow window("Shader Drawable",
                      geom::isize(960, 600),
                      geom::isize(960, 600));
  TextureManager    texture_manager;
  SurfaceManager    surface_manager;
  DrawableGroup group;

  int have_stencil = 0xdeadbeaf;
  SDL_GL_GetAttribute(SDL_GL_STENCIL_SIZE, &have_stencil);
  std::cout << "STENCIL: " << have_stencil << std::endl;

  std::shared_ptr<ShaderDrawable>  shader(new ShaderDrawable());
  std::shared_ptr<ShaderDrawable>  shader2(new ShaderDrawable());
  std::shared_ptr<SurfaceDrawable> surface(new SurfaceDrawable(surface_manager.get(Pathname("images/titlescreen.png")),
                                                               SurfaceDrawingParameters(), 0, glm::mat4(1.0f)));
  std::shared_ptr<SurfaceDrawable> surface2(new SurfaceDrawable(surface_manager.get(Pathname("images/menu_background.jpg")),
                                                                SurfaceDrawingParameters().set_pos(glm::vec2(-50.0f, -50.0f)),
                                                                0, glm::mat4(1.0f)));
  std::shared_ptr<SurfaceDrawable> surface3(new SurfaceDrawable(surface_manager.get(Pathname("images/portrait.png")),
                                                                SurfaceDrawingParameters(), 0, glm::mat4(1.0f)));
  std::shared_ptr<SurfaceDrawable> surface4(new SurfaceDrawable(surface_manager.get(Pathname("images/portrait.png")),
                                                                SurfaceDrawingParameters().set_pos(glm::vec2(-25.0f, 25.0f)),
                                                                0, glm::mat4(1.0f)));

  TexturePtr texture = texture_manager.get(Pathname("images/displacement.png"));

  ShaderProgramPtr shader_prg;
  shader_prg->attach(ShaderObject::create_from_file(GL_FRAGMENT_SHADER_ARB, "data/shader/border.frag"));
  shader_prg->link();

  shader->set_shader(shader_prg);
  shader->get_drawable_group().add_drawable(surface3);

  ShaderProgramPtr shader2_prg;
  shader2_prg->attach(ShaderObject::create_from_file(GL_FRAGMENT_SHADER_ARB, "data/shader/nightvision.frag"));
  shader2_prg->link();

  shader2->set_shader(shader2_prg);
  shader2->get_drawable_group().add_drawable(surface2);

  group.add_drawable(shader2);
  group.add_drawable(shader);

  for(int i = 0; i < 500; ++i)
  {
    std::cout << "." << std::flush;
    surface3->get_params().set_pos(glm::vec2(static_cast<float>(i), 50.0f));
    group.render(window.get_gc(), ~0u);
    window.swap_buffers();
  }

  return 0;
}

} // namespace

int main(int argc, char** argv)
{
  try {
    return shader_main(argc, argv);
  } catch(std::exception const& err) {
    std::cerr << "exception: " << err.what() << std::endl;
  } catch(...) {
    std::cerr << "unknown exception" << std::endl;
  }
}

/* EOF */
