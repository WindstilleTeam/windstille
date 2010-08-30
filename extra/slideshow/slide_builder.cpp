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

#include "slideshow/slide_builder.hpp"

#include <boost/tokenizer.hpp>
#include <boost/lexical_cast.hpp>
#include <string.h>
#include <errno.h>
#include <fstream>
#include <iostream>
#include <stdexcept>
#include <sstream>

#include "slideshow/slide_show.hpp"

float
NodePosX::get(const Sizef& scr, const Sizef& img) const
{
  switch(m_type)
  {
    case kNodePosXLeft:
      return img.width/2.0f - scr.width/2.0f + scr.width/2.0f;

    case kNodePosXRight:
      return -img.width/2.0f + scr.width/2.0f + scr.width/2.0f;

    case kNodePosXCenter:
      return scr.width/2;

    case kNodePosXFloat:
      return m_value;
  }

  assert(!"never reached");
}

float
NodePosY::get(const Sizef& scr, const Sizef& img) const
{
  switch(m_type)
  {
    case kNodePosYTop:
      return img.height/2.0f - scr.height/2.0f + scr.height/2.0f;
      
    case kNodePosYBottom:
      return -img.height/2.0f + scr.height/2.0f + scr.height/2.0f;

    case kNodePosYCenter:
      return scr.height/2;
      
    case kNodePosYFloat:
      return m_value;
  }

  assert(!"never reached");
}

float
NodeZoom::get(const Sizef& scr, const Sizef& img) const
{
  switch(m_type)
  {
    case kNodeZoomFit:
      return std::min(scr.width / img.width,
                      scr.height / img.height);

    case kNodeZoomFill:
      return std::max(scr.width / img.width,
                      scr.height / img.height);

    case kNodeZoomWidth:
      return scr.width / img.width;
      
    case kNodeZoomHeight:
      return scr.height / img.height;
      
    case kNodeZoomOriginal:
      return 1.0f;

    case kNodeZoomFloat:
      {
      // 1.0f means "fit", so recalculate values relative to that
      float fit = std::min(scr.width / img.width,
                           scr.height / img.height);
      return fit * m_value;
      }
  }

  assert(!"never reached");
}

SlideBuilder::SlideBuilder(SlideShow& slideshow, const Sizef& screen_size) :
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
  m_path_node(),
  m_node()
{
}

void
SlideBuilder::load_from_file(const std::string& filename)
{
  std::ifstream in(filename.c_str());
  if (!in)
  {
    throw std::runtime_error(strerror(errno));
  }
  else
  {
    m_context = filename;
    m_line = 0;
    load_from_stream(in);
  }
}

std::string
strip_comment(const std::string& line)
{
  std::string::size_type p = line.find('#');
  return line.substr(0, p);
}

void
SlideBuilder::error(const std::string& str)
{
  std::ostringstream out;
  out << m_context << ':' << m_line << ": error: " << str;
  throw std::runtime_error(out.str());
}

void
SlideBuilder::load_from_stream(std::istream& stream)
{
  try 
  {
    std::string line;
    while(std::getline(stream, line))
    {
      m_line += 1;
      line = strip_comment(line);

      boost::char_separator<char> sep(" \t");
      boost::tokenizer<boost::char_separator<char> > tokens(line, sep);
      std::vector<std::string> args(tokens.begin(), tokens.end());

      if (args.empty())
      {
        // empty line, skip it
      }
      else
      {
        if (false)
        { // debug output
          for(std::vector<std::string>::iterator i = args.begin(); i != args.end(); ++i)
          {
            std::cout << "(" << *i << ") ";
          }
          std::cout << std::endl;
        }

        if (args[0] == "image")
        {
          handle_image(args);
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
SlideBuilder::handle_image(const std::vector<std::string>& args)
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
    std::cout << ";; time = " << m_time << std::endl;
    std::cout << "(image \"" << args[1] << "\")" << std::endl;
    m_image = SlideObjectPtr(new SlideObject(Pathname(args[1], Pathname::kSysPath)));

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
SlideBuilder::handle_pos(const std::vector<std::string>& args)
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
  else if (!m_node_has_zoom)
  {
    error("zoom must come before pos");
  }
  else
  {
    m_node_has_pos = true;

    float img_w = m_image->get_width()  * m_path_node.zoom;
    float img_h = m_image->get_height() * m_path_node.zoom;

    float scr_w = m_screen_size.width;
    float scr_h = m_screen_size.height;

    if (args[1] == "left")
    {
      m_path_node.pos.x = img_w/2.0f - scr_w/2.0f + scr_w/2.0f;
    }
    else if (args[1] == "right")
    {
      m_path_node.pos.x = -img_w/2.0f + scr_w/2.0f + scr_w/2.0f;
    }
    else if (args[1] == "center")
    {
      m_path_node.pos.x = scr_w/2;
    }
    else
    {
      float x = boost::lexical_cast<float>(args[1]) * m_path_node.zoom;
      m_path_node.pos.x = img_w/2.0f - x + scr_w/2.0f;
    }

    if (args[2] == "top")
    {
      m_path_node.pos.y = img_h/2.0f - scr_h/2.0f + scr_h/2.0f;
    }
    else if (args[2] == "bottom")
    {
      m_path_node.pos.y = -img_h/2.0f + scr_h/2.0f + scr_h/2.0f;
    }
    else if (args[2] == "center")
    {
      m_path_node.pos.y = scr_h/2;
    }
    else
    {
      float y = boost::lexical_cast<float>(args[2]) * m_path_node.zoom;
      m_path_node.pos.y = img_h/2.0f - y + scr_h/2.0f;
    }

    std::cout << "  (pos " << m_path_node.pos.x << " " << m_path_node.pos.y << ")" << std::endl;
  }
}

void
SlideBuilder::handle_zoom(const std::vector<std::string>& args)
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
      m_path_node.zoom = std::min(m_screen_size.width / m_image->get_width(),
                                  m_screen_size.height / m_image->get_height());
    }
    else if (args[1] == "fill")
    {
      m_path_node.zoom = std::max(m_screen_size.width / m_image->get_width(),
                                  m_screen_size.height / m_image->get_height());
    }
    else if (args[1] == "width")
    {
      m_path_node.zoom = m_screen_size.width / m_image->get_width();
    }
    else if (args[1] == "height")
    {
      m_path_node.zoom = m_screen_size.height / m_image->get_height();
    }
    else if (args[1] == "original")
    {
      m_path_node.zoom = 1.0f;
    }
    else
    {
      m_path_node.zoom = boost::lexical_cast<float>(args[1]);
      
      // 1.0f means "fit", so recalculate values relative to that
      float fit = std::min(m_screen_size.width / m_image->get_width(),
                           m_screen_size.height / m_image->get_height());
      m_path_node.zoom = fit * m_path_node.zoom;

      std::cout << "  (zoom " << m_path_node.zoom << ")" << std::endl;
    }

    m_node_has_zoom = true;
  }
}
 
void
SlideBuilder::handle_duration(const std::vector<std::string>& args)
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
    float duration = boost::lexical_cast<float>(args[1]);

    assert(m_image);
    add_node();
    m_image->get_path().add_edge(duration);
    std::cout << "  (duration " << duration << ")" << std::endl;
  }
}

void
SlideBuilder::add_node()
{
  if (!m_node_has_pos)
  {
    error("getting default pos not implemented");
  }

  if (!m_node_has_zoom)
  {
    error("getting default zoom not implemented");
  }

  Sizef m_image_size(m_image->get_width(),
                     m_image->get_height());

  m_image->get_path().add_node(m_path_node.pos, m_path_node.zoom);
  //m_image->get_path().add_node(Vector2f(m_node.pos_x.get(m_screen_size, m_image_size),
  //                                     m_node.pos_y.get(m_screen_size, m_image_size)),
  //                           m_node.zoom.get(m_screen_size, m_image_size));

  m_node_has_zoom = false;
  m_node_has_pos  = false;
}

void
SlideBuilder::handle_fade(const std::vector<std::string>& args)
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
    m_fade = boost::lexical_cast<float>(args[1]);
    std::cout << "(fade " << m_fade << ")" << std::endl;

    if (m_last_image)
    {
      m_last_image->set_fade_out(m_fade);
    }
  }
}

void
SlideBuilder::handle_end(const std::vector<std::string>& args)
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
SlideBuilder::handle_breakpoint(const std::vector<std::string>& args)
{
  std::cout << "breakpoint not implemented" << std::endl;
}

void
SlideBuilder::handle_include(const std::vector<std::string>& args)
{
  
}

/* EOF */
