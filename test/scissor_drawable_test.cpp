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

int main(int argc, char* argv[])
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
                      Size(960, 600),
                      Size(960, 600));
  TextureManager    texture_manager;
  SurfaceManager    surface_manager;
  DrawableGroup group;

  int have_stencil = 0xdeadbeaf;
  SDL_GL_GetAttribute(SDL_GL_STENCIL_SIZE, &have_stencil);
  std::cout << "STENCIL: " << have_stencil << std::endl;

  boost::shared_ptr<ScissorDrawable> scissor(new ScissorDrawable(Rect(200, 200, 400, 400)));
  boost::shared_ptr<StencilDrawable> stencil(new StencilDrawable());
  boost::shared_ptr<SurfaceDrawable> surface(new SurfaceDrawable(Surface::create(Pathname("images/titlescreen.png")),
                                                                 SurfaceDrawingParameters(), 0, Matrix(1.0f)));

  boost::shared_ptr<SurfaceDrawable> surface2(new SurfaceDrawable(Surface::create(Pathname("images/titlescreen.png")),
                                                                  SurfaceDrawingParameters().set_pos(Vector2f(-50.0f, -50.0f)), 0, Matrix(1.0f)));
  boost::shared_ptr<SurfaceDrawable> surface3(new SurfaceDrawable(Surface::create(Pathname("images/portrait.png")),
                                                                  SurfaceDrawingParameters(), 0, Matrix(1.0f)));

  boost::shared_ptr<SurfaceDrawable> surface4(new SurfaceDrawable(Surface::create(Pathname("images/portrait.png")),
                                                                  SurfaceDrawingParameters().set_pos(Vector2f(-25.0f, 25.0f)),
                                                                  0, Matrix(1.0f)));
  boost::shared_ptr<StencilDrawable> stencil2(new StencilDrawable());

  if (0)
  {
    scissor->get_drawable_group().add_drawable(surface);
    surface2->set_pos(Vector2f(300.0f, 300.0f));
    surface3->set_pos(Vector2f(350.0f, 350.0f));
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
    surface->get_params().set_pos(Vector2f(static_cast<float>(i) - 250.0f, 0.0f));
    group.render(~0u);
    window.swap_buffers();
  }

  return 0;
}

/* EOF */
