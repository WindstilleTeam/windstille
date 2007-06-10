/*  $Id$
**   __      __ __             ___        __   __ __   __
**  /  \    /  \__| ____    __| _/_______/  |_|__|  | |  |   ____
**  \   \/\/   /  |/    \  / __ |/  ___/\   __\  |  | |  | _/ __ \
**   \        /|  |   |  \/ /_/ |\___ \  |  | |  |  |_|  |_\  ___/
**    \__/\  / |__|___|  /\____ /____  > |__| |__|____/____/\___  >
**         \/          \/      \/    \/                         \/
**  Copyright (C) 2005 Ingo Ruhnke <grumbel@gmx.de>
**
**  This program is free software; you can redistribute it and/or
**  modify it under the terms of the GNU General Public License
**  as published by the Free Software Foundation; either version 2
**  of the License, or (at your option) any later version.
**
**  This program is distributed in the hope that it will be useful,
**  but WITHOUT ANY WARRANTY; without even the implied warranty of
**  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
**  GNU General Public License for more details.
** 
**  You should have received a copy of the GNU General Public License
**  along with this program; if not, write to the Free Software
**  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA
**  02111-1307, USA.
*/

#include <boost/format.hpp>
#include <iostream>
#include <unistd.h>
#include "game_session.hpp"
#include "input/input_manager_sdl.hpp"
#include "display/display.hpp"
#include "globals.hpp"
#include "screen.hpp"
#include "font/fonts.hpp"
#include "config.hpp"
#include "input/input_manager.hpp"
#include "input/input_configurator.hpp"
#include "sound/sound_manager.hpp"
#include "gui/gui_manager.hpp"
#include "screen_manager.hpp"

// GUI Stuff, can be removed if gui is a bit better organised
#include "gui/button.hpp"
#include "gui/slider.hpp"
#include "gui/root_component.hpp"
#include "gui/grid_component.hpp"
#include "gui/menu_component.hpp"
#include "gui/tab_component.hpp"
#include "gui/list_view.hpp"
#include "gui/text_view.hpp"
#include "gui/automap.hpp"


using GUI::GUIManager;

ScreenManager screen_manager; 

ScreenManager::ScreenManager()
  : time_counter(0),
    frame_counter(0),
    last_fps(0),
    overlap_delta(0),
    do_quit(false)
{
  screen = 0;
  overlay_screen = 0;
  next_overlay_screen = 0;
  has_next_overlay_screen = false;
  ticks = 0;
}

ScreenManager::~ScreenManager()
{
}

void
ScreenManager::run()
{
  do_quit = false;

  ticks = SDL_GetTicks();

  while (!do_quit)
    {
      /// Amount of time the world moves forward each update(), this is
      /// independed of the number of frames and always constant
      static const float step = .001;

      Uint32 now = SDL_GetTicks();
      float delta = static_cast<float>(now - ticks) / 1000.0f + overlap_delta;
      ticks = now;

      time_counter  += delta;

      while (delta > step)
        {
          InputManager::update(delta);

          console.update(step);
          if (!console.is_active())
            {
              if (overlay_screen)
                overlay_screen->update(step, InputManager::get_controller());
              else
                screen->update(step, InputManager::get_controller());
            }
          InputManager::clear();
  
          delta -= step;
        }

      overlap_delta = delta;

      sound_manager->update();

      screen->draw();

      if (overlay_screen)
        overlay_screen->draw();

      console.draw();

      if (config.get_bool("show-fps"))
        draw_fps();

      SDL_GL_SwapBuffers();
      frame_counter += 1;

      if (has_next_overlay_screen)
        {
          delete overlay_screen;
          overlay_screen = next_overlay_screen;
          next_overlay_screen = 0;
          has_next_overlay_screen = false;
        }

      poll_events();
    }
}

void
ScreenManager::poll_events()
{
  SDL_Event event;
  while(SDL_PollEvent(&event))
    {
      switch(event.type)
        {
        case SDL_QUIT:
          // FIXME: This should be a bit more gentle, but will do for now
          std::cout << "Ctrl-c or Window-close pressed, game is going to quit" << std::endl;
          quit();
          break;
          
        case SDL_ACTIVEEVENT:
          // event.active
          break;
          
        case SDL_VIDEORESIZE:
          // event.resize
          break;
              
        case SDL_VIDEOEXPOSE:
          // event.expose
          break;
                
        case SDL_USEREVENT:
          // event.user
          break;
                    
        case SDL_SYSWMEVENT:
          // event.syswm
          break;
          break;

        case SDL_KEYDOWN:
        case SDL_KEYUP:
          if (event.key.state)
            {    
              switch (event.key.keysym.sym)
                {       
                case SDLK_F6:
                  SDL_ShowCursor(SDL_ENABLE);   // SDL_ENABLE to show the mouse cursor (default)
                  SDL_WM_GrabInput(SDL_GRAB_OFF); // SDL_GRAB_OFF to not grab input (default)
                  break;

                case SDLK_F7:
                  SDL_ShowCursor(SDL_DISABLE);   // SDL_ENABLE to show the mouse cursor (default)
                  SDL_WM_GrabInput(SDL_GRAB_ON); // SDL_GRAB_OFF to not grab input (default)
                  break;
      
                case SDLK_F8:
                  {
                    using namespace GUI;
                    GUIManager* manager = new GUIManager();

                    TabComponent* tab = new TabComponent(Rectf(100, 100, 700, 500), manager->get_root());

                    GridComponent* grid = new GridComponent(Rectf(100, 130, 700, 500), 3, 4, tab);

                    grid->pack(new Button("1", grid), 0, 0);
                    grid->pack(new Button("2", grid), 1, 0);
                    grid->pack(new Button("3", grid), 2, 0);

                    grid->pack(new Slider(grid), 0, 1);
                    //grid->pack(new Button("5", grid), 1, 1, 2, 2);
                    TextView* text_view = new TextView(Rectf(), grid);
                    grid->pack(text_view, 1, 1, 2, 2);

                    //grid->pack(new Button("6", grid), 2, 1);

                    grid->pack(new Button("7", grid), 0, 2, 1, 2);
                    //grid->pack(new Button("8", grid), 1, 2);
                    //grid->pack(new Button("9", grid), 2, 2);

                    //grid->pack(new Button("Cl", grid), 0, 3);
                    grid->pack(new Button("0",  grid), 1, 3);
                    grid->pack(new Button("Ok", grid), 2, 3);

                    // Begin Option Menu
                    MenuComponent* menu = new MenuComponent(Rectf(100, 130, 700, 500), tab);

                    SliderMenuItem* music_volume_item = new SliderMenuItem(menu, "Music Volume", 100, 0, 100, 10);
                    menu->add_item(music_volume_item);

                    SliderMenuItem* sfx_volume_item = new SliderMenuItem(menu, "Sound FX Volume", 100, 0, 100, 10);
                    menu->add_item(sfx_volume_item);

                    EnumMenuItem* fullscreen_item = new EnumMenuItem(menu, "Fullscreen", 0);
                    fullscreen_item->add_pair(1, "on");
                    fullscreen_item->add_pair(0, "off");
                    menu->add_item(fullscreen_item);

                    EnumMenuItem* fps_item = new EnumMenuItem(menu, "Display Frames per Second", 0);
                    fps_item->add_pair(1, "on");
                    fps_item->add_pair(0, "off");
                    menu->add_item(fps_item);


                    EnumMenuItem* aspect_item = new EnumMenuItem(menu, "Aspect Ratio");
                    aspect_item->add_pair(0, "4:3");
                    aspect_item->add_pair(1, "16:9");
                    aspect_item->add_pair(3, "16:10");
                    aspect_item->add_pair(2, "letterbox");
                    menu->add_item(aspect_item);

                    tab->pack("Options", menu);
                    // End: Option Menu

                    tab->pack("Auto Map",  new Automap(Rectf(100, 130, 700, 500), tab));
                    tab->pack("Grid Test", grid);

                    ListView* list_view = new ListView(Rectf(), tab);
                    list_view->add_column("Date");
                    list_view->add_column("Name");
                    list_view->add_column("Subject");

                    list_view->add_item(ListView::Item("2005-10-08", "John Doh", "Re: Buying a goldmine"));
                    list_view->add_item(ListView::Item("2005-13-08", "Jane Doh", "Re: What the f***"));
                    list_view->add_item(ListView::Item("2005-13-09", "Testo Test", "Testing Email"));

                    tab->pack("ListView", list_view);

                    manager->get_root()->set_child(tab);
                    text_view->set_text("Hello World\n<large>Blabla</large> more textt and more and"
                                        "more for testing all for testing even more and more blabla blabla"
                                        "more for testing all for testing even more and more blabla blabla"
                                        "blabla blabla blabltest ende.");

                    set_overlay(manager);
                  }
                  break;

                case SDLK_F9:
                  set_overlay(new InputConfigurator());
                  break;

                case SDLK_F10:
                  config.set_bool("show-fps", !config.get_bool("show-fps"));
                  break;
              
                case SDLK_F11:
                  config.set_bool("fullscreen", !config.get_bool("fullscreen"));
                  Display::set_fullscreen(config.get_bool("fullscreen"));
                  break;
              
                case SDLK_F12:
                  {
                    // FIXME: Replace this with Physfs stuff
                    int count = 0;
                    std::string filename;
                    do {
                      filename = (boost::format("/tmp/windstille%04d.png") % count).str();
                      count += 1;
                    } while(access(filename.c_str(), F_OK) == 0);

                    Display::save_screenshot(filename);
                    console << "Writing screenshot to: '" << filename << "'" << std::endl;
                  }
                  break;
              
                default:
                  if (!console.is_active())
                    {
                      if (overlay_screen)
                        overlay_screen->handle_event(event);
                      else
                        screen->handle_event(event);
                    }
                  break;
                }
            }
              
          if (!console.is_active() && event.key.state && event.key.keysym.sym == SDLK_F1)
            {
              console.activate();
            }
          else
            {
              if (InputManagerSDL::current())
                InputManagerSDL::current()->on_event(event);
            }
          break;

        case SDL_MOUSEBUTTONUP:
        case SDL_MOUSEBUTTONDOWN:
        case SDL_MOUSEMOTION:
        case SDL_JOYAXISMOTION:
        case SDL_JOYBALLMOTION:
        case SDL_JOYHATMOTION:
        case SDL_JOYBUTTONUP:
        case SDL_JOYBUTTONDOWN:
          if (InputManagerSDL::current())
            InputManagerSDL::current()->on_event(event);

          if (overlay_screen)
            overlay_screen->handle_event(event);
          break;
        
        default:
          if (overlay_screen)
            overlay_screen->handle_event(event);
          else              
            screen->handle_event(event);
          break;
        }
    }
}

void 
ScreenManager::draw_fps()
{
  if(time_counter > 1) 
    {
      last_fps = int(frame_counter / time_counter);

      time_counter  = fmodf(time_counter, 1.0);
      frame_counter = 0;
    }
  
  char output[20];
  snprintf(output, sizeof(output), "FPS: %d", last_fps);
  Fonts::ttffont->draw(Display::get_width() - 100, 30, output);
}

void
ScreenManager::set_screen(Screen* s)
{
  delete screen;
  screen = s;
}

void
ScreenManager::set_overlay(Screen* s)
{
  next_overlay_screen = s;
  has_next_overlay_screen = true;
}

void
ScreenManager::quit()
{
  do_quit = true;
}

/* EOF */
