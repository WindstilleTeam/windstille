#include <iostream>
#include <sstream>
#include <stdexcept>
#include "SDL.h"

#include "display/opengl_window.hpp"
#include "display/surface.hpp"
#include "display/surface_manager.hpp"
#include "display/texture_manager.hpp"
#include "scenegraph/drawable_group.hpp"
#include "scenegraph/surface_drawable.hpp"
#include "scenegraph/scissor_drawable.hpp"
#include "scenegraph/stencil_drawable.hpp"
#include "util/pathname.hpp"
#include "util/system.hpp"

namespace {

int app_main(int argc, char* argv[])
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

  OpenGLWindow window("Scissor Drawable",
                      geom::isize(960, 600),
                      geom::isize(960, 600));
  TextureManager    texture_manager;
  SurfaceManager    surface_manager;
  DrawableGroup group;

  int have_stencil = 0xdeadbeaf;
  SDL_GL_GetAttribute(SDL_GL_STENCIL_SIZE, &have_stencil);
  std::cout << "STENCIL: " << have_stencil << std::endl;

  std::shared_ptr<ScissorDrawable> scissor(new ScissorDrawable(geom::irect(200, 200, 400, 400)));
  std::shared_ptr<StencilDrawable> stencil(new StencilDrawable());
  std::shared_ptr<SurfaceDrawable> surface(new SurfaceDrawable(surface_manager.get(Pathname("images/titlescreen.png")),
                                                               SurfaceDrawingParameters(), 0, glm::mat4(1.0f)));

  std::shared_ptr<SurfaceDrawable> surface2(new SurfaceDrawable(surface_manager.get(Pathname("images/titlescreen.png")),
                                                                SurfaceDrawingParameters().set_pos(glm::vec2(-50.0f, -50.0f)), 0, glm::mat4(1.0f)));
  std::shared_ptr<SurfaceDrawable> surface3(new SurfaceDrawable(surface_manager.get(Pathname("images/portrait.png")),
                                                                SurfaceDrawingParameters(), 0, glm::mat4(1.0f)));

  std::shared_ptr<SurfaceDrawable> surface4(new SurfaceDrawable(surface_manager.get(Pathname("images/portrait.png")),
                                                                SurfaceDrawingParameters().set_pos(glm::vec2(-25.0f, 25.0f)),
                                                                0, glm::mat4(1.0f)));
  std::shared_ptr<StencilDrawable> stencil2(new StencilDrawable());

  if ((false))
  {
    scissor->get_drawable_group().add_drawable(surface);
    surface2->set_pos(glm::vec2(300.0f, 300.0f));
    surface3->set_pos(glm::vec2(350.0f, 350.0f));
    group.add_drawable(scissor);
  }
  else
  {
    stencil->get_stencil_group().add_drawable(surface3);
    stencil->get_drawable_group().add_drawable(surface);

    group.add_drawable(surface2);

    stencil2->get_stencil_group().add_drawable(surface4);
    stencil2->get_drawable_group().add_drawable(stencil);

    group.add_drawable(stencil2);
  }

  for(int i = 0; i < 500; ++i)
  {
    std::cout << "." << std::flush;
    surface->get_params().set_pos(glm::vec2(static_cast<float>(i) - 250.0f, 0.0f));
    group.render(~0u);
    window.swap_buffers();
  }

  return 0;
}

} // namespace

int main(int argc, char** argv)
{
  try {
    return app_main(argc, argv);
  } catch(std::exception const& err) {
    std::cerr << "exception: " << err.what() << std::endl;
  } catch(...) {
    std::cerr << "unknown exception" << std::endl;
  }
}

/* EOF */
