/*
**  Windstille - A Sci-Fi Action-Adventure Game
**  Copyright (C) 2021 Ingo Ruhnke <grumbel@gmail.com>
**
**  This program is free software: you can redistribute it and/or modify
**  it under the terms of the GNU General Public License as published by
**  the Free Software Foundation, either version 3 of the License, or
**  (at your option) any later version.
**
**  This program is distributed in the hope that it will be useful,
**  but WITHOUT ANY WARRANTY; without even the implied warranty of
**  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
**  GNU General Public License for more details.
**
**  You should have received a copy of the GNU General Public License
**  along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <SDL.h>

#include <glm/glm.hpp>
#include <glm/ext.hpp>

#include <geom/io.hpp>
#include <surf/palette.hpp>
#include <wstdisplay/font/ttf_font_manager.hpp>
#include <wstdisplay/opengl_window.hpp>
#include <wstgui/anchor_layoutable.hpp>
#include <wstgui/box_layoutable.hpp>
#include <wstgui/button.hpp>
#include <wstgui/gui_manager.hpp>
#include <wstgui/ilayoutable.hpp>
#include <wstgui/label.hpp>
#include <wstgui/layout_builder.hpp>
#include <wstgui/menu.hpp>
#include <wstgui/root_component.hpp>
#include <wstgui/screen_manager.hpp>
#include <wstgui/style.hpp>
#include <wstgui/text_view.hpp>
#include <wstinput/controller.hpp>
#include <wstinput/controller_description.hpp>
#include <wstinput/input_manager.hpp>
#include <wstsystem/system.hpp>

using namespace wstinput;
using namespace wstdisplay;
using namespace wstgui;

int main(int argc, char** argv)
{
  wstsys::System system;

  auto window = system.create_window({
      .title = "GUI Test",
      .icon = {},
      .size = geom::isize(640, 480),
      .resizable = true
    });

  ControllerDescription controller_description;
  controller_description.add_pointer("pointer-x", 0);
  controller_description.add_pointer("pointer-y", 1);

  InputManagerSDL input(controller_description);
  input.bindings().bind_mouse_motion(0, 0, 0);
  input.bindings().bind_mouse_motion(1, 0, 1);

  ScreenManager screen(system, *window, input);

  TTFFontManager ttf_font_manager;
  std::unique_ptr<TTFFont> font = ttf_font_manager.create_font("extra/Vera.ttf", 20);
  std::unique_ptr<TTFFont> smallfont = ttf_font_manager.create_font("extra/Vera.ttf", 16);
  Style style(font.get());
  auto gui_manager = std::make_unique<GUIManager>(style);
  auto* root = gui_manager->get_root();

  auto label = std::make_unique<Label>("GUI Test", root);
  label->set_geometry(geom::frect(20, 20, 100, 50));
  root->add_child(std::move(label));

  auto textview = std::make_unique<TextView>(root);
  textview->set_geometry(geom::frect(20, 50, 620, 200));
  textview->set_font(smallfont.get());
  textview->set_text(
    "Lorem ipsum dolor sit amet, consectetur adipiscing elit. Morbi in "
    "consectetur sapien. Pellentesque a cursus est. Vestibulum nec "
    "ultricies sapien, et tristique dui. Donec pulvinar turpis sed risus "
    "eleifend, eget malesuada tortor lacinia. Aliquam ut dolor purus. "
    "Mauris porta diam a iaculis aliquet. Phasellus maximus arcu sed nisi "
    "efficitur viverra. Donec sed libero sed dui varius sollicitudin eu non "
    "elit. Quisque eu arcu ornare, feugiat urna non, consectetur odio. Duis "
    "lobortis et leo at finibus. Aliquam condimentum ligula id nulla "
    "facilisis mollis.\n"
    "\n"
    "Duis vulputate consequat ipsum in suscipit. Nunc porttitor convallis "
    "eros, sit amet gravida justo lobortis nec. Vivamus vehicula velit eu "
    "nisl fringilla, at posuere sem placerat. Class aptent taciti sociosqu "
    "ad litora torquent per conubia nostra, per inceptos himenaeos. Proin "
    "vel magna risus. Integer ullamcorper, dui eu fringilla sollicitudin, "
    "nisi lorem varius massa, sit amet vehicula sem lorem nec nulla. Nam "
    "volutpat condimentum mi in vestibulum. Maecenas malesuada risus non "
    "lectus egestas blandit. Nullam convallis nunc quis tellus vulputate "
    "congue. Cras id diam vitae nisi rhoncus congue. Sed nisi neque, ornare "
    "a leo et, vehicula condimentum sapien. Maecenas maximus consequat "
    "tellus vel condimentum. Maecenas imperdiet elit quis enim auctor "
    "lacinia.\n"
    "\n"
    "Phasellus vel suscipit massa. Phasellus in magna nec magna placerat "
    "egestas. Nullam id condimentum lacus, ac bibendum est. Donec sit amet "
    "facilisis tellus. Ut molestie non tortor sit amet porta. Duis feugiat "
    "scelerisque ipsum non hendrerit. Aenean vel odio faucibus, pharetra "
    "felis id, pharetra felis. Donec tempus augue lorem, at suscipit tortor "
    "aliquet et. Phasellus molestie justo in odio dignissim, sit amet "
    "molestie ex convallis. Suspendisse vitae tortor sapien.\n"
    );
  root->add_child(std::move(textview));

  auto ok_button = std::make_unique<Button>("Ok", root);
  auto cancel_button = std::make_unique<Button>("Cancel", root);
  ok_button->set_geometry(geom::frect(500, 420, 600, 460));
  cancel_button->set_geometry(geom::frect(380, 420, 480, 460));
  root->add_child(std::move(ok_button));
  root->add_child(std::move(cancel_button));

  auto menu = std::make_unique<wstgui::Menu>("Options", geom::frect(geom::fpoint(100, 100), geom::fsize(500, 340)), style);
  menu->add_slider("Master Volume",  50, 0, 100, 10, {});
  menu->add_slider("Music Volume", 75, 100, 10, {});
  menu->add_slider("Sound Volume", 100, 0, 100, 10, {});
  menu->add_slider("Voice Volume", 50, 0, 100, 10, {});
  //root->add_child(std::move(menu));

  auto button1 = std::make_unique<Button>("Button1", root);
  auto button2 = std::make_unique<Button>("Button2", root);
  auto button3 = std::make_unique<Button>("Button3", root);
  auto button4 = std::make_unique<Button>("Button4", root);
  auto button5 = std::make_unique<Button>("Button5", root);
  auto button6 = std::make_unique<Button>("Button6", root);

  auto vbox = std::make_unique<wstgui::VBox>();
  auto hbox = std::make_unique<wstgui::HBox>();
  vbox->pack(button1.get());
  vbox->pack_spacer(5.0f);
  hbox->pack(button2.get());
  hbox->pack_spacer(5.0f);
  hbox->pack(button3.get());
  hbox->pack_stretcher();
  hbox->pack(button4.get());
  hbox->pack_stretcher();
  hbox->pack(button5.get(), false);
  hbox->pack(button6.get(), false);
  vbox->pack(std::move(hbox));
  vbox->set_geometry({50, 200, 500, 400});

  root->add_child(std::move(button1));
  root->add_child(std::move(button2));
  root->add_child(std::move(button3));
  root->add_child(std::move(button4));
  root->add_child(std::move(button5));
  root->add_child(std::move(button6));

  auto btn1 = std::make_unique<Button>("Button1", root);
  auto btn2 = std::make_unique<Button>("Button2", root);
  auto btn3 = std::make_unique<Button>("Button3", root);
  auto btn4 = std::make_unique<Button>("Button4", root);
  auto btn5 = std::make_unique<Button>("Button5", root);
  auto btn6 = std::make_unique<Button>("Button6", root);
  auto btn7 = std::make_unique<Button>("Button7", root);

  auto anchor = std::make_unique<AnchorLayoutable>();

  anchor->place(btn1.get(),
                rfloat::relative(0.0f), rfloat::relative(0.0f),
                rfloat::absolute(150.0f), rfloat::absolute(100.0f),
                geom::origin::TOP_LEFT);
  anchor->place(btn2.get(),
                rfloat::relative(1.0f), rfloat::relative(0.0f),
                rfloat::absolute(150.0f), rfloat::absolute(100.0f),
                geom::origin::TOP_RIGHT);
  anchor->place(btn3.get(),
                rfloat::relative(0.5f), rfloat::relative(0.5f),
                rfloat::absolute(150.0f), rfloat::absolute(100.0f),
                geom::origin::CENTER);
  anchor->place(btn4.get(),
                rfloat::relative(0.0f), rfloat::relative(1.0f),
                rfloat::relative(0.25f), rfloat::absolute(100.0f),
                geom::origin::BOTTOM_LEFT);
  anchor->place(btn5.get(),
                rfloat::relative(1.0f), rfloat::relative(1.0f),
                rfloat::relative(0.25f), rfloat::absolute(100.0f),
                geom::origin::BOTTOM_RIGHT);
  anchor->place(btn6.get(),
                rfloat::relative(0.5f), rfloat::relative(0.5f),
                rfloat::relative(0.25f), rfloat::relative(0.25f),
                geom::origin::CENTER);
  anchor->place(btn7.get(), geom::frect(300, 300, 350, 330));

  root->add_child(std::move(btn1));
  root->add_child(std::move(btn2));
  root->add_child(std::move(btn3));
  root->add_child(std::move(btn4));
  root->add_child(std::move(btn5));
  root->add_child(std::move(btn6));
  root->add_child(std::move(btn7));

  anchor->set_geometry(geom::frect(geom::fsize(window->get_size())));

  window->sig_resized.connect([&](geom::isize const& size){
    vbox->set_geometry(geom::grow(geom::frect(geom::fpoint(0.0f, 0.0f),
                                              geom::fsize(size)), -100.0f));
    anchor->set_geometry(geom::frect(geom::fsize(size)));
    glViewport(0, 0, size.width(), size.height());
    auto& gc = window->get_gc();
    gc.set_ortho(size);
  });

  screen.push_screen(std::move(gui_manager));

  screen.bind_key(SDLK_ESCAPE, [&]{
    screen.quit();
  });

  screen.sig_draw_begin().connect([](wstdisplay::GraphicsContext& gc){
    gc.clear(surf::palette::black);
  });
  screen.run();

#if 0
  bool quit = false;
  while (!quit) {
    system.update();

    auto& gc = window->get_gc();
    gc.clear(surf::palette::black);
    gui_manager->draw(gc);
    menu->get_root()->draw(gc);
    window->swap_buffers();

    gui_manager->update(1.0f / 60, Controller());
  }
#endif

  return 0;
}

/* EOF */
