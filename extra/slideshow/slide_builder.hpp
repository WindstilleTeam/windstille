/*
**  Windstille - A Sci-Fi Action-Adventure Game
**  Copyright (C) 2010 Ingo Ruhnke <grumbel@gmx.de>
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

#ifndef HEADER_WINDSTILLE_EXTRA_SLIDESHOW_SLIDE_BUILDER_HPP
#define HEADER_WINDSTILLE_EXTRA_SLIDESHOW_SLIDE_BUILDER_HPP

#include <string>
#include <vector>

#include "math/size.hpp"
#include "slideshow/slide_object.hpp"
#include "slideshow/slide_path.hpp"

class SlideShow;

class SlideBuilder
{
private:
  SlideShow& m_slideshow;
  Sizef m_screen_size;

  std::string m_context;
  int m_line;

  enum State {
    kGlobal,
    kImage
  };

  float m_fade;

  State m_state;
  float m_time;
  SlideObjectPtr m_image;
  SlideObjectPtr m_last_image;

  bool m_node_has_pos;
  bool m_node_has_zoom;
  SlidePathNode m_path_node;

public:
  SlideBuilder(SlideShow& slideshow, const Sizef& screen_size);

  void load_from_file(const std::string& filename);
  void load_from_stream(std::istream& stream);

private:
  void error(const std::string& str);

  void handle_image(const std::vector<std::string>& args);
  void handle_pos(const std::vector<std::string>& args);
  void handle_zoom(const std::vector<std::string>& args);
  void handle_duration(const std::vector<std::string>& args);
  void handle_fade(const std::vector<std::string>& args);
  void handle_end(const std::vector<std::string>& args);

  void add_node();

private:
  SlideBuilder(const SlideBuilder&);
  SlideBuilder& operator=(const SlideBuilder&);
};

#endif

/* EOF */
