#include <config.h>

#include "sprite2d/data.hpp"

#include <physfs.h>
#include <iostream>
#include <memory>
#include <sstream>
#include <stdexcept>
#include "util.hpp"
#include "lisp/lisp.hpp"
#include "lisp/parser.hpp"
#include "lisp/properties.hpp"
#include "lisp_getters.hpp"
#include "display/surface.hpp"
#include "display/surface_manager.hpp"

namespace sprite2d {

static bool has_suffix(const std::string& str, const std::string& suffix)
{
  if (str.length() >= suffix.length())
    return str.compare(str.length() - suffix.length(), suffix.length(), suffix) == 0;
  else
    return false;
}

static std::string tolowercase(const std::string& str)
{
  std::string out;
  for(std::string::const_iterator i = str.begin();  i != str.end(); ++i)
    out += tolower(*i);
  return out;
}

Data::Data(const std::string& filename)
{
  if (PHYSFS_exists(filename.c_str()))
    {
      if (has_suffix(filename, ".sprite"))
        {
          std::auto_ptr<lisp::Lisp> root (lisp::Parser::parse(filename));
          lisp::Properties rootp(root.get());
          const lisp::Lisp* sprite = 0;
          if(rootp.get("sprite", sprite) == false) {
            std::ostringstream msg;
            msg << "File '" << filename << "' is not a windstille sprite";
            throw std::runtime_error(msg.str());
          }
    
          std::string dir = dirname(filename);
          parse(dir, sprite);
        }
      else if (has_suffix(tolowercase(filename), ".png") || has_suffix(tolowercase(filename), ".jpg"))
        {
          if (PHYSFS_exists(filename.c_str()))
            {
              std::auto_ptr<Action> action(new Action);
              action->name   = "default";
              action->speed  = 1.0;
              action->scale  = 1.0f;
              action->offset = Vector(0, 0);
              action->surfaces.push_back(Surface(filename));
              actions.push_back(action.release());
            }
          else
            {
              throw std::runtime_error("Couldn't find '" + filename + "'");
            }
        }
      else
        {
          throw std::runtime_error("Sprite filename has unknown suffix: '" + filename + "'");
        }
    }
  else if (filename.length() > std::string(".sprite").length())
    { // If sprite file is not found, we search for a file with the
      // same name ending in .png
      std::string pngfile = filename.substr(0, filename.length() - std::string(".sprite").length()) + ".png";

      if (PHYSFS_exists(pngfile.c_str()))
        {
          std::auto_ptr<Action> action(new Action);
          action->name   = "default";
          action->speed  = 1.0;
          action->scale  = 1.0f;
          action->offset = Vector(0, 0);
          action->surfaces.push_back(Surface(pngfile));
          actions.push_back(action.release());
        }
      else
        {
          throw std::runtime_error("Couldn't find '" + filename + "'");
        }
    }
  else
    {
      throw std::runtime_error("Couldn't find '" + filename + "'");
    }
}

Data::~Data()
{
  for(Actions::iterator i = actions.begin(); i != actions.end(); ++i)
    delete *i;
}

void
Data::parse(const std::string& dir, const lisp::Lisp* lisp)
{
  lisp::Properties props(lisp);
  lisp::PropertyIterator<const lisp::Lisp*> iter;
  props.get_iter("action", iter);
  while(iter.next()) {
    actions.push_back(parse_action(dir, *iter));
  }
  props.print_unused_warnings("sprite");

  if(actions.size() == 0)
    throw std::runtime_error("Sprite contains no actions");
}

Action*
Data::parse_action(const std::string& dir, const lisp::Lisp* lisp)
{
  std::auto_ptr<Action> action (new Action);
  action->speed = 1.0;
  action->scale = 1.0f;
  action->offset = Vector(0, 0);
 
  lisp::Properties props(lisp);
  props.get("name", action->name);
  props.get("speed", action->speed);
  props.get("scale", action->scale);
  props.get("offset", action->offset);
  
  const lisp::Lisp* ilisp = 0;
  if(props.get("images", ilisp)) {
    parse_images(action.get(), dir, ilisp);
  } else if(props.get("image-grid", ilisp)) {
    parse_image_grid(action.get(), dir, ilisp);
  }
  props.print_unused_warnings("sprite action");
  
  if(action->name == "")
    throw std::runtime_error("No Name defined for action");
  if(action->surfaces.size() == 0) {
    std::ostringstream msg;
    msg << "Action '" << action->name << "' contains no images";
    throw std::runtime_error(msg.str());
  }
  return action.release();
}

void
Data::parse_images(Action* action, const std::string& dir,
                   const lisp::Lisp* lisp)
{
  for(size_t n = 1; n < lisp->get_list_size(); ++n) {
    std::string file = lisp->get_list_elem(n)->get_string();
    Surface surface = surface_manager->get(dir + "/" + file);
    action->surfaces.push_back(surface);
  }
}

void
Data::parse_image_grid(Action* action, const std::string& dir,
                       const lisp::Lisp* lisp)
{
  std::string filename;
  int x_size = -1;
  int y_size = -1;
 
  lisp::Properties props(lisp);

  props.get("file", filename);
  props.get("x-size", x_size);
  props.get("y-size", y_size);

  props.print_unused_warnings("action image-grid");

  if(filename == "" || x_size <= 0 || y_size <= 0)
    throw std::runtime_error("Invalid or too few data in image-grid");

  surface_manager->load_grid(dir + "/" + filename,
                             action->surfaces, x_size, y_size);
}
 
} // namespace sprite2d

/* EOF */
