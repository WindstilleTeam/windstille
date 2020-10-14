/*
**  Windstille - A Sci-Fi Action-Adventure Game
**  Copyright (C) 2010 Ingo Ruhnke <grumbel@gmail.com>
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
#include <map>

#include <geom/geom.hpp>

#include "slideshow/slide_object.hpp"
#include "slideshow/slide_path.hpp"

class SlideShow;

class NodePosX
{
public:
  enum NodePosXType
  {
    kNodePosXLeft,
    kNodePosXRight,
    kNodePosXCenter,
    kNodePosXAbsFloat,
    kNodePosXRelFloat
  };

public:
  NodePosX() :
    m_type(kNodePosXCenter),
    m_value(0.0f)
  {}

  NodePosX(NodePosXType type, float value = 0.0f) :
    m_type(type),
    m_value(value)
  {}

  float get(const geom::fsize& screen_size, const geom::fsize& image_size, float zoom) const;

private:
  NodePosXType m_type;
  float m_value;
};

class NodePosY
{
public:
  enum NodePosYType
  {
    kNodePosYTop,
    kNodePosYBottom,
    kNodePosYCenter,
    kNodePosYAbsFloat,
    kNodePosYRelFloat
  };

public:
  NodePosY() :
    m_type(kNodePosYCenter),
    m_value(0.0f)
  {}

  NodePosY(NodePosYType type, float value = 0.0f) :
    m_type(type),
    m_value(value)
  {}

  float get(const geom::fsize& screen_size, const geom::fsize& image_size, float zoom) const;

private:
  NodePosYType m_type;
  float m_value;
};

class NodeZoom
{
public:
  enum NodeZoomType
  {
    kNodeZoomOriginal,
    kNodeZoomFit,
    kNodeZoomFill,
    kNodeZoomWidth,
    kNodeZoomHeight,
    kNodeZoomFloat
  };

public:
  NodeZoom() :
    m_type(kNodeZoomFit),
    m_value(0.0f)
  {}

  NodeZoom(NodeZoomType type, float value = 1.0f) :
    m_type(type),
    m_value(value)
  {}

  float get(const geom::fsize& screen_size, const geom::fsize& image_size) const;

private:
  NodeZoomType m_type;
  float m_value;
};

class Node
{
public:
  NodeZoom zoom;
  NodePosX pos_x;
  NodePosY pos_y;

  Node() :
    zoom(),
    pos_x(),
    pos_y()
  {}
};

class SlideParser
{
private:
  SlideShow& m_slideshow;
  geom::fsize m_screen_size;

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
  //SlidePathNode m_path_node;
  Node m_node;

  typedef std::map<std::string, std::string> Variables;
  Variables m_variables;

  SurfaceManager& m_surface_manager;

public:
  SlideParser(SlideShow& slideshow, const geom::fsize& screen_size,
              SurfaceManager& surface_manager);

  void load_from_file(const std::string& filename);
  void load_from_stream(std::istream& stream);

private:
  [[noreturn]]
  void error(const std::string& str) const;

  void handle_image(const std::vector<std::string>& args);
  void handle_pos(const std::vector<std::string>& args);
  void handle_zoom(const std::vector<std::string>& args);
  void handle_duration(const std::vector<std::string>& args);
  void handle_fade(const std::vector<std::string>& args);
  void handle_end(const std::vector<std::string>& args);
  void handle_breakpoint(const std::vector<std::string>& args);
  void handle_include(const std::vector<std::string>& args);
  void handle_set(const std::vector<std::string>& args);

  void add_node();

  std::vector<std::string> tokenize(const std::string& str) const;

private:
  SlideParser(const SlideParser&);
  SlideParser& operator=(const SlideParser&);
};

#endif

/* EOF */
