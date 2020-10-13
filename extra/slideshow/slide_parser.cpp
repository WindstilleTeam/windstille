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

#include "slideshow/slide_parser.hpp"

#include <string.h>
#include <errno.h>
#include <fstream>
#include <iostream>
#include <stdexcept>
#include <sstream>

#include "slideshow/slide_show.hpp"

namespace {

std::vector<std::string> string_tokenize(std::string_view text, std::string_view delimiter)
{
  auto is_delimiter = [delimiter](char c) -> bool {
    return std::find(delimiter.begin(), delimiter.end(), c) != delimiter.end();
  };

  std::vector<std::string> result;

  for(std::string::size_type i = 0; i != text.size();)
  {
    while(is_delimiter(text[i]) && i != text.size()) { ++i; }
    const std::string::size_type start = i;
    while(!is_delimiter(text[i]) && i != text.size()) { ++i; }
    const std::string::size_type end = i;
    if (start != end) {
      result.emplace_back(text.substr(start, end - start));
    }
  }

  return result;
}


} // namespace

float
NodePosX::get(const Sizef& scr, const Sizef& img, float zoom) const
{
  // FIXME: Warning img is already multiplied by zoom
  switch(m_type)
  {
    case kNodePosXLeft:
      return img.width()/2.0f - scr.width()/2.0f + scr.width()/2.0f;

    case kNodePosXRight:
      return -img.width()/2.0f + scr.width()/2.0f + scr.width()/2.0f;

    case kNodePosXCenter:
      return scr.width()/2;

    case kNodePosXAbsFloat:
      {
        float x = m_value * zoom;
        return img.width()/2.0f - x + scr.width()/2.0f;
      }

    case kNodePosXRelFloat:
      {
        float x = (m_value/100.0f) * img.width();
        return img.width()/2.0f - x + scr.width()/2.0f;
      }

    default:
      assert(false && "never reached");
      return 0.0f;
  }
}

float
NodePosY::get(const Sizef& scr, const Sizef& img, float zoom) const
{
  // FIXME: Warning img is already multiplied by zoom
  switch(m_type)
  {
    case kNodePosYTop:
      return img.height()/2.0f - scr.height()/2.0f + scr.height()/2.0f;

    case kNodePosYBottom:
      return -img.height()/2.0f + scr.height()/2.0f + scr.height()/2.0f;

    case kNodePosYCenter:
      return scr.height()/2;

    case kNodePosYAbsFloat:
      {
        float y = m_value * zoom;
        return img.height()/2.0f - y + scr.height()/2.0f;
      }

    case kNodePosYRelFloat:
      {
        float y = (m_value/100.0f) * img.height();
        return img.height()/2.0f - y + scr.height()/2.0f;
      }

    default:
      assert(false && "never reached");
      return 0.0f;
  }
}

float
NodeZoom::get(const Sizef& scr, const Sizef& img) const
{
  switch(m_type)
  {
    case kNodeZoomFit:
      return std::min(scr.width() / img.width(),
                      scr.height() / img.height());

    case kNodeZoomFill:
      return std::max(scr.width() / img.width(),
                      scr.height() / img.height());

    case kNodeZoomWidth:
      return scr.width() / img.width();

    case kNodeZoomHeight:
      return scr.height() / img.height();

    case kNodeZoomOriginal:
      return 1.0f;

    case kNodeZoomFloat:
      {
        // 1.0f means "fit", so recalculate values relative to that
        float fit = std::min(scr.width() / img.width(),
                             scr.height() / img.height());
        return fit * m_value;
      }

    default:
      assert(false && "never reached");
      return 0.0f;
  }
}

SlideParser::SlideParser(SlideShow& slideshow, const Sizef& screen_size,
                         SurfaceManager& surface_manager) :
  m_slideshow(slideshow),
  m_screen_size(screen_size),
  m_context(),
  m_line(),
  m_fade(0.0f),
  m_state(kGlobal),
  m_time(0.0f),
  m_image(),
  m_last_image(),
  m_node_has_pos(false),
  m_node_has_zoom(false),
  //m_path_node(),
  m_node(),
  m_variables(),
  m_surface_manager(surface_manager)
{
}

void
SlideParser::load_from_file(const std::string& filename)
{
  std::ifstream in(filename.c_str());
  if (!in)
  {
    std::ostringstream str;
    str << filename << ": " << strerror(errno);
    throw std::runtime_error(str.str());
  }
  else
  {
    m_context = filename;
    m_line = 0;
    load_from_stream(in);
  }
}

namespace {

std::string
strip_comment(const std::string& line)
{
  std::string::size_type p = line.find('#');
  return line.substr(0, p);
}

} // namespace

void
SlideParser::error(const std::string& str) const
{
  std::ostringstream out;
  out << m_context << ':' << m_line << ": error: " << str;
  throw std::runtime_error(out.str());
}

void
SlideParser::load_from_stream(std::istream& stream)
{
  try
  {
    std::string line;
    while(std::getline(stream, line))
    {
      m_line += 1;
      line = strip_comment(line);

      std::vector<std::string> args = tokenize(line);

      if (args.empty())
      {
        // empty line, skip it
      }
      else
      {
        if ((false))
        { // debug output
          for(std::vector<std::string>::iterator i = args.begin(); i != args.end(); ++i)
          {
            std::cout << "(" << *i << ") ";
          }
          std::cout << std::endl;
        }

        // handle variable replacement
        for(std::vector<std::string>::iterator i = args.begin(); i != args.end(); ++i)
        {
          if ((*i)[0] == '$')
          {
            Variables::iterator it = m_variables.find(i->substr(1));
            if (it == m_variables.end())
            {
              error("unknown variable: " + *i);
            }
            else
            {
              *i = it->second;
            }
          }
        }

        if (args[0] == "image")
        {
          handle_image(args);
        }
        else if (args[0] == "set")
        {
          handle_set(args);
        }
        else if (args[0] == "zoom")
        {
          handle_zoom(args);
        }
        else if (args[0] == "pos")
        {
          handle_pos(args);
        }
        else if (args[0] == "duration")
        {
          handle_duration(args);
        }
        else if (args[0] == "fade")
        {
          handle_fade(args);
        }
        else if (args[0] == "end")
        {
          handle_end(args);
        }
        else if (args[0] == "breakpoint")
        {
          handle_breakpoint(args);
        }
        else if (args[0] == "include")
        {
          handle_include(args);
        }
        else
        {
          error("unknown tag '" + args[0] + "'");
        }
      }
    }

    if (m_state != kGlobal)
    {
      error("not in global scope at end of file");
    }

    std::cout << "Total time: " << m_time << std::endl;
  }
  catch(std::exception& err)
  {
    error(err.what());
  }
}

void
SlideParser::handle_image(const std::vector<std::string>& args)
{
  if (m_state != kGlobal)
  {
    error("image must be in global scope");
  }
  else if (args.size() != 2)
  {
    error("image requires one argument");
  }
  else
  {
    m_state = kImage;

    // reset pan/zoom pos
    m_node = Node();

    std::cout << ";; time = " << m_time << std::endl;
    std::cout << "(image \"" << args[1] << "\")" << std::endl;
    m_image = SlideObjectPtr(new SlideObject(args[1], m_surface_manager));

    if (m_slideshow.size() != 0)
    {
      m_time -= m_fade;
      m_image->set_begin(m_time);
    }

    m_image->set_fade_in(m_fade);
    m_fade = 0.0f;
  }
}

void
SlideParser::handle_pos(const std::vector<std::string>& args)
{
  if (m_state != kImage)
  {
    error("pos must be in image scope");
  }
  else if (args.size() != 3)
  {
    error("pos requires two arguments");
  }
  else if (m_node_has_pos)
  {
    error("pos already given");
  }
  else
  {
    m_node_has_pos = true;

    //float img_w = m_image->get_width()  * m_path_node.zoom;
    //float img_h = m_image->get_height() * m_path_node.zoom;

    //float scr_w = m_screen_size.width;
    //float scr_h = m_screen_size.height;

    if (args[1] == "left")
    {
      //m_path_node.pos.x = img_w/2.0f - scr_w/2.0f + scr_w/2.0f;
      m_node.pos_x = NodePosX(NodePosX::kNodePosXLeft);
    }
    else if (args[1] == "right")
    {
      //m_path_node.pos.x = -img_w/2.0f + scr_w/2.0f + scr_w/2.0f;
      m_node.pos_x = NodePosX(NodePosX::kNodePosXRight);
    }
    else if (args[1] == "center")
    {
      //m_path_node.pos.x = scr_w/2;
      m_node.pos_x = NodePosX(NodePosX::kNodePosXCenter);
    }
    else if (args[1] == "prev")
    {
      // reuse previous value
    }
    else
    {
      //float x = std::stof(args[1]) * m_path_node.zoom;
      //m_path_node.pos.x = img_w/2.0f - x + scr_w/2.0f;
      if (args[1][args[1].length()-1] == '%')
      {
        m_node.pos_x = NodePosX(NodePosX::kNodePosXRelFloat,
                                std::stof(args[1].substr(0, args[1].length()-1)));
      }
      else
      {
        m_node.pos_x = NodePosX(NodePosX::kNodePosXAbsFloat,
                                std::stof(args[1]));
      }
    }

    if (args[2] == "top")
    {
      //m_path_node.pos.y = img_h/2.0f - scr_h/2.0f + scr_h/2.0f;
      m_node.pos_y = NodePosY(NodePosY::kNodePosYTop);
    }
    else if (args[2] == "bottom")
    {
      //m_path_node.pos.y = -img_h/2.0f + scr_h/2.0f + scr_h/2.0f;
      m_node.pos_y = NodePosY(NodePosY::kNodePosYBottom);
    }
    else if (args[2] == "center")
    {
      //m_path_node.pos.y = scr_h/2;
      m_node.pos_y = NodePosY(NodePosY::kNodePosYCenter);
    }
    else if (args[2] == "prev")
    {
      // reuse previous value
    }
    else
    {
      //float y = std::stof(args[2]) * m_path_node.zoom;
      //m_path_node.pos.y = img_h/2.0f - y + scr_h/2.0f;
      if (args[2][args[2].length()-1] == '%')
      {
        m_node.pos_y = NodePosY(NodePosY::kNodePosYRelFloat,
                                std::stof(args[2].substr(0, args[2].length()-1)));
      }
      else
      {
        m_node.pos_y = NodePosY(NodePosY::kNodePosYAbsFloat,
                                std::stof(args[2]));
      }
    }

    //std::cout << "  (pos " << m_path_node.pos.x << " " << m_path_node.pos.y << ")" << std::endl;
  }
}

void
SlideParser::handle_zoom(const std::vector<std::string>& args)
{
  if (m_state != kImage)
  {
    error("zoom must be in image scope");
  }
  else if (args.size() != 2)
  {
    error("zoom requires one argument");
  }
  else if (m_node_has_zoom)
  {
    error("zoom already given");
  }
  else
  {
    if (args[1] == "fit")
    {
      //m_path_node.zoom = std::min(m_screen_size.width / m_image->get_width(),
      //                            m_screen_size.height / m_image->get_height());
      m_node.zoom = NodeZoom(NodeZoom::kNodeZoomFit);
    }
    else if (args[1] == "fill")
    {
      //m_path_node.zoom = std::max(m_screen_size.width / m_image->get_width(),
      //m_screen_size.height / m_image->get_height());
      m_node.zoom = NodeZoom(NodeZoom::kNodeZoomFill);
    }
    else if (args[1] == "width")
    {
      //m_path_node.zoom = m_screen_size.width / m_image->get_width();
      m_node.zoom = NodeZoom(NodeZoom::kNodeZoomWidth);
    }
    else if (args[1] == "height")
    {
      //m_path_node.zoom = m_screen_size.height / m_image->get_height();
      m_node.zoom = NodeZoom(NodeZoom::kNodeZoomHeight);
    }
    else if (args[1] == "original")
    {
      //m_path_node.zoom = 1.0f;
      m_node.zoom = NodeZoom(NodeZoom::kNodeZoomOriginal);
    }
    else if (args[1] == "prev")
    {
      // reuse previous value
    }
    else
    {
      /*
      m_path_node.zoom = std::stof(args[1]);

      // 1.0f means "fit", so recalculate values relative to that
      float fit = std::min(m_screen_size.width / m_image->get_width(),
                           m_screen_size.height / m_image->get_height());
      m_path_node.zoom = fit * m_path_node.zoom;

      std::cout << "  (zoom " << m_path_node.zoom << ")" << std::endl;
      */
      m_node.zoom = NodeZoom(NodeZoom::kNodeZoomFloat, std::stof(args[1]));
    }

    m_node_has_zoom = true;
  }
}

void
SlideParser::handle_duration(const std::vector<std::string>& args)
{
  if (m_state != kImage)
  {
    error("duration must be in image scope");
  }
  else if (args.size() != 2)
  {
    error("duration requires one argument");
  }
  else
  {
    float duration = std::stof(args[1]);

    assert(m_image);
    add_node();
    m_image->get_path().add_edge(duration);
    std::cout << "  (duration " << duration << ")" << std::endl;
  }
}

void
SlideParser::add_node()
{
  Sizef image_size(m_image->get_width(), m_image->get_height());
  float zoom = m_node.zoom.get(m_screen_size, image_size);
  Sizef image_size_zoom(zoom * m_image->get_width(),
                        zoom * m_image->get_height());

  //m_image->get_path().add_node(m_path_node.pos, m_path_node.zoom);
  m_image->get_path().add_node(glm::vec2(m_node.pos_x.get(m_screen_size, image_size_zoom, zoom),
                                        m_node.pos_y.get(m_screen_size, image_size_zoom, zoom)),
                               zoom);

  m_node_has_zoom = false;
  m_node_has_pos  = false;
}

void
SlideParser::handle_fade(const std::vector<std::string>& args)
{
  if (m_state != kGlobal)
  {
    error("fade must be in global scope");
  }
  else if (args.size() != 2)
  {
    error("fade requires one argument");
  }
  else
  {
    m_fade = std::stof(args[1]);
    std::cout << "(fade " << m_fade << ")" << std::endl;

    if (m_last_image)
    {
      m_last_image->set_fade_out(m_fade);
    }
  }
}

void
SlideParser::handle_end(const std::vector<std::string>& args)
{
  if (m_state != kImage)
  {
    error("end must be in image scope");
  }
  else if (args.size() != 1)
  {
    error("end requires no argument");
  }
  else
  {
    m_state = kGlobal;

    assert(m_image);
    add_node();
    m_image->get_path().finish();

    m_time += m_image->length();
    m_slideshow.add(m_image);
    m_last_image = m_image;
    std::cout << "(end)" << std::endl;
  }
}

void
SlideParser::handle_breakpoint(const std::vector<std::string>& args)
{
  std::cout << "breakpoint not implemented" << std::endl;
}

void
SlideParser::handle_include(const std::vector<std::string>& args)
{
  std::cout << "include not implemented" << std::endl;
}

void
SlideParser::handle_set(const std::vector<std::string>& args)
{
  if (args.size() != 3)
  {
    error("set requires exactly one argument");
  }
  else
  {
    m_variables[args[1]] = args[2];
  }
}

std::vector<std::string>
SlideParser::tokenize(const std::string& line) const
{
  if ((true))
  {
    return string_tokenize(line, " \t");
  }
  else
  {
    //enum { kWhitespace, kString } state;

    std::vector<std::string> lst;
    std::ostringstream str;
    std::string::const_iterator i = line.begin();

    while(i != line.end())
    {

      if (*i == ' ' || *i == '\t')
      {

      }
      else if (*i == '\\')
      {
        ++i;
        if (i != line.end())
        {
          switch(*i)
          {
            case '\\': str << '\\'; break;
            case '0': str << '\0'; break;
            case 'a': str << '\a'; break;
            case 'b': str << '\b'; break;
            case 't': str << '\t'; break;
            case 'r': str << '\r'; break;
            case 'n': str << '\n'; break;
            default: str << '\\' << *i; break;
          }
        }
        else
        {
          error("couldn't tokenize line");
        }
      }
      else
      {
      }

      ++i;
    }

    return lst;
  }
}

/* EOF */
