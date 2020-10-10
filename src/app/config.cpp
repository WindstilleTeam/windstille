/*
**  Windstille - A Sci-Fi Action-Adventure Game
**  Copyright (C) 2000,2005 Ingo Ruhnke <grumbel@gmail.com>
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

#include "app/config.hpp"

#include <iostream>
#include <fstream>
#include <fmt/format.h>
#include <stdio.h>
#include <logmich/log.hpp>

#include <argparser.hpp>

#include "app/windstille.hpp"
#include "util/sexpr_file_reader.hpp"
#include "app/globals.hpp"

#define _(A) A

std::vector<std::string> arg_files;

Config config;

Config::Config()
  : config_values()
{
  add(new ConfigValue<int> ("anti-aliasing",  _("Use NUMx Anti-Aliasing"), true, 0));
  add(new ConfigValue<bool>("fullscreen",     _("Use fullscreen"),         true, false));
  add(new ConfigValue<bool>("show-fps",       _("Show frames per second"), true, true));

  add(new ConfigValue<bool>("music",          _("Enable Music"), true, true));
  add(new ConfigValue<bool>("sound",          _("Enable Sound"), true, true));

  add(new ConfigValue<int>("aspect-width",    _("Aspect Width"),   true, 1280));
  add(new ConfigValue<int>("aspect-height",   _("Aspect Height"),  true,  800));

  add(new ConfigValue<int>("screen-width",    _("Screen Width"),   true, 1280));
  add(new ConfigValue<int>("screen-height",   _("Screen Height"),  true,  800));

  add(new ConfigValue<std::string>("levelfile",       _("Levelfile to be used at startup"), false));

  // FIXME: There is no need to limit this to just two
  add(new ConfigValue<std::string>("primary-controller-file",   _("Controller Config file to load"), true));
  add(new ConfigValue<std::string>("secondary-controller-file", _("Controller Config file to load"), true));

  add(new ConfigValue<std::string>("screenshot-dir",  _("Directory where Screenshots are saved"), false));

  add(new ConfigValue<std::string>("recorder-file",   _("File to which demos are recorded"), false));
  add(new ConfigValue<std::string>("playback-file",   _("File from which a demo is played"), false));

  add(new ConfigValue<int>("master-volume",  _("Master Volume"), true, 100));
  add(new ConfigValue<int>("music-volume",   _("Music Volume"),  true, 100));
  add(new ConfigValue<int>("sound-volume",   _("Sound Volume"),  true, 100));
  add(new ConfigValue<int>("voice-volume",   _("Voice Volume"),  true, 100));

  add(new ConfigValue<bool>("wiimote", _("Try to connect to Wiimote on startup"), true, false));
}

Config::~Config()
{
  for(ConfigValues::iterator i = config_values.begin(); i != config_values.end(); ++i)
  {
    delete i->second;
  }
  config_values.clear();
}

void
Config::add(ConfigValueBase* value)
{
  config_values[value->get_name()] = value;
}

std::string
Config::get_string(const std::string& name) const
{
  return get<std::string>(name).get();
}

bool
Config::get_bool(const std::string& name) const
{
  return get<bool>(name).get();
}

int
Config::get_int(const std::string& name) const
{
  return get<int>(name).get();
}

float
Config::get_float(const std::string& name) const
{
  return get<float>(name).get();
}

void
Config::set_string(const std::string& name, const std::string& value)
{
  get<std::string>(name).set(value);
}

void
Config::set_bool  (const std::string& name, bool  value)
{
  get<bool>(name).set(value);
}

void
Config::set_int   (const std::string& name, int   value)
{
  get<int>(name).set(value);
}

void
Config::set_float (const std::string& name, float value)
{
  get<float>(name).set(value);
}

void
Config::parse_args(int argc, char** argv)
{
  arg_files.clear();
  const int secondary_controller_file = 261;

  argparser::ArgParser argp;
  argp.add_usage(argv[0], "[LEVELFILE]")
    .add_text("Windstille is a classic Jump'n Run game.");

  argp.add_group("Display Options:")
    .add_option('g', "geometry",   "WxH", "Change window size to WIDTH and HEIGHT")
    .add_option('a', "aspect",   "WxH", "Change aspect size to WIDTH and HEIGHT")
    .add_option('f', "fullscreen", "", "Launch the game in fullscreen")
    .add_option('A', "anti-aliasing", "NUM", "Enable NUMx Anti-Aliasing");

  argp.add_group("Sound Options:")
    .add_option('s', "disable-sound", "", "Disable sound")
    .add_option('S', "enable-sound", "", "Enable sound");

  argp.add_group("Controlls Options:")
    .add_option('c', "controller", "FILE", "Use controller as defined in FILE")
    .add_option(secondary_controller_file, '\0', "secondary-controller", "FILE",
                "Use controller as defined in FILE")
#ifdef HAVE_CWIID
    .add_option('w', "wiimote", "", "Connect to Wiimote on startup")
#endif
    ;

  argp.add_group("Misc Options:")
    .add_option('d', "datadir",    "DIR", "Fetch game data from DIR")
    .add_option('v', "version",       "", "Print Windstille Version")
    .add_option('h', "help",       "", "Print this help");

  argp.add_group("Demo Recording/Playback Options:")
    .add_option('r', "record",      "FILE", "Record input events to FILE")
    //.add_option('x', "record-video","DIR",  "Record a gameplay video to DIR")
    .add_option('p', "play",        "FILE", "Playback input events from FILE");

  argp.parse_args(argc, argv);

  for(auto const& opt : argp.parse_args(argc, argv))
  {
    switch (opt.key)
    {
      case 'A':
        int anti_aliasing;
        if (sscanf(opt.argument.c_str(), "%d", &anti_aliasing) != 1)
        {
          throw std::runtime_error("Anti-Aliasing option '-a' requires argument of type {NUM}");
        }
        else
        {
          get<int>("anti-aliasing") = anti_aliasing;
        }
        break;

      case 'r':
        get<std::string>("recorder-file") = opt.argument;
        break;

      case 'x':
        get<std::string>("screenshot-dir") = opt.argument;
        break;

      case 'p':
        get<std::string>("playback-file") = opt.argument;
        break;

      case 'w':
        get<bool>("wiimote") = true;
        break;

      case 'd':
        Pathname::set_datadir(opt.argument);
        break;

      case 'f':
        set_bool("fullscreen", true);
        break;

      case 'g':
        {
          int screen_width  = 800;
          int screen_height = 600;
          if (sscanf(opt.argument.c_str(), "%dx%d",
                     &screen_width, &screen_height) == 2)
          {
            get<int>("screen-width")  = screen_width;
            get<int>("screen-height") = screen_height;

            // FIXME: Why does this get printed twice?!
            // Is the argument parser buggy?
            std::cout << "Geometry: " << screen_width << "x" << screen_height << std::endl;
          }
          else
          {
            throw std::runtime_error("Geometry option '-g' requires argument of type {WIDTH}x{HEIGHT}");
          }
        }
        break;

      case 'a':
        {
          int aspect_width  = 800;
          int aspect_height = 600;
          if (sscanf(opt.argument.c_str(), "%dx%d",
                     &aspect_width, &aspect_height) == 2)
          {
            get<int>("aspect-width")  = aspect_width;
            get<int>("aspect-height") = aspect_height;

            // FIXME: Why does this get printed twice?!
            // Is the argument parser buggy?
            std::cout << "Geometry: " << aspect_width << "x" << aspect_height << std::endl;
          }
          else
          {
            throw std::runtime_error("Geometry option '-a' requires argument of type {WIDTH}x{HEIGHT}");
          }
        }
        break;

      case 's':
        set_bool("sound", false);
        break;

      case 'S':
        set_bool("sound", true);
        break;

      case 'c':
        get<std::string>("primary-controller-file") = opt.argument;
        break;

      case secondary_controller_file:
        get<std::string>("secondary-controller-file") = opt.argument;
        break;

      case 'v':
        std::cout << "Windstille " << WINDSTILLE_VERSION << std::endl;
        exit(EXIT_SUCCESS);
        break;

      case 'h':
        argp.print_help();
        exit(EXIT_SUCCESS);
        break;

      case argparser::ArgumentType::REST:
        set_string("levelfile", opt.argument);
        arg_files.push_back(opt.argument);
        break;
    }
  }
}

bool
Config::has_key(const std::string& name)
{
  ConfigValues::iterator i = config_values.find(name);
  return (i != config_values.end());
}

bool
Config::is_set(const std::string& name)
{
  ConfigValues::iterator i = config_values.find(name);
  if (i == config_values.end())
  {
    throw std::runtime_error("Error: unknown Config value: '" + name + "'");
  }
  else
  {
    return i->second->is_set();
  }
}

void
Config::load()
{
  try
  {
    FileReader reader = FileReader::parse(Pathname("config", Pathname::kUserPath));
    if(reader.get_name() != "windstille-config")
    {
      std::cerr << "Warning: Config file is not a windstille-config file.\n";
      return;
    }

    for(ConfigValues::iterator i = config_values.begin(); i != config_values.end(); ++i)
    { // FIXME: all this dynamic_casting is overcomplicated crap
      if (dynamic_cast<ConfigValue<int>*>(i->second))
      {
        int v;
        if (reader.get(i->first.c_str(), v))
          set_int(i->first, v);
      }
      else if (dynamic_cast<ConfigValue<bool>*>(i->second))
      {
        bool v;
        if (reader.get(i->first.c_str(), v))
          set_bool(i->first, v);
      }
      else if (dynamic_cast<ConfigValue<float>*>(i->second))
      {
        float v;
        if (reader.get(i->first.c_str(), v))
          set_float(i->first, v);
      }
      else if (dynamic_cast<ConfigValue<std::string>*>(i->second))
      {
        std::string v;
        if (reader.get(i->first.c_str(), v))
          set_string(i->first, v);
      }
      else
      {
        std::cout << "Config: Unknown type for: " << i->first << std::endl;
      }
    }

    // TODO read controller config
  }
  catch(std::exception& e)
  {
    std::cerr << "Couldn't open config file: " << e.what() << "\n"
              << "This is normal on first startup!\n";
    return;
  }
}

void
Config::save()
{
  try {
    FileWriter writer = FileWriter::from_file(Pathname("config", Pathname::kUserPath).get_sys_path());

    writer.write_comment("Windstille Config - automatically read and written on startup/quit");
    writer.begin_object("windstille-config");

    for(ConfigValues::iterator i = config_values.begin(); i != config_values.end(); ++i)
    {
      if (i->second->should_be_saved() && i->second->is_set())
      {
        writer.write_comment(i->second->get_docstring());
        i->second->write(writer);
      }
    }
    // TODO write controller config

    writer.end_object();
  } catch(std::exception& e) {
    std::cerr << "Couldn't write config file: " << e.what() << "\n";
  }
}

void
Config::debug_print(std::ostream& out)
{
  out << "Config " << this << ":" << std::endl;
  for(ConfigValues::iterator i = config_values.begin(); i != config_values.end(); ++i)
  {
    out << fmt::format("  {:20} = {:-20} ({})",
                       i->second->get_name(),
                       (fmt::format("'{:1s}'", (*i->second).str())),
                       (i->second->is_set() ? "set" : "default"))
                       << std::endl;

  }
}

template<>
void ConfigValue<bool>::write(FileWriter& writer) {
  writer.write(get_name(), data);
}

template<>
void ConfigValue<int>::write(FileWriter& writer) {
  writer.write(get_name(), data);
}

template<>
void ConfigValue<float>::write(FileWriter& writer) {
  writer.write(get_name(), data);
}

template<>
void ConfigValue<std::string>::write(FileWriter& writer) {
  writer.write(get_name(), data);
}

/* EOF */

