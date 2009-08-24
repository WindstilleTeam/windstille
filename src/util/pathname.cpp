/*
**  Windstille - A Sci-Fi Action-Adventure Game
**  Copyright (C) 2009 Ingo Ruhnke <grumbel@gmx.de>
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

#include "util/pathname.hpp"

#include <sstream>
#include <boost/filesystem.hpp>

std::string Pathname::s_datadir;
std::string Pathname::s_userdir;
Pathname::Overrides Pathname::s_datadir_overrides;

void
Pathname::set_datadir_overrides(const Overrides& overrides)
{
  s_datadir_overrides = overrides;
}

static void add_overrides(const boost::filesystem::path& path, const boost::filesystem::path& base, 
                         Pathname::Overrides* overrides)
{
  boost::filesystem::directory_iterator end_i; // default construction yields past-the-end

  for(boost::filesystem::directory_iterator i(path); i != end_i; ++i)
  {
    if (boost::filesystem::is_directory(i->status()))
    {
      add_overrides(i->path(), base / i->leaf(), overrides);
    }
    else
    {
      std::string from_path   = (base / i->leaf()).string();
      std::string to_path    = i->path().native_file_string();
      (*overrides)[from_path] = to_path;
    }
  }
}

void
Pathname::add_datadir_overrides(const std::string& path)
{
  add_overrides(path, "", &s_datadir_overrides);
}

void Pathname::clear_datadir_overrides()
{
  s_datadir_overrides.clear();
}

void Pathname::set_datadir(const std::string& datadir)
{
  s_datadir = datadir;
}

void Pathname::set_userdir(const std::string& userdir)
{
  s_userdir = userdir;
}

std::string Pathname::get_datadir()
{
  return s_datadir;
}

std::string Pathname::get_userdir()
{
  return s_userdir;
}

// -----------------------------------------------------------------------------

Pathname::Pathname(const std::string& path, PathType type)
  : m_path(path),
    m_type(type)
{  
}

bool
Pathname::exists() const
{
  switch(m_type)
  {
    case kEmpty:
      return false;

    default:
      return boost::filesystem::exists(get_sys_path());
  }
}

std::string
Pathname::get_physfs_path() const
{
  switch(m_type)
  {
    case kSysPath: 
    {
      std::ostringstream str;
      str << "Can't convert " << *this << " to PhysFS path";
      throw std::runtime_error(str.str()); 
      return "";
    }
          
    case kUserPath:
    case kDataPath:
      return m_path;

    case kEmpty:
      return "";

    default:
      assert(!"Never reached");
      return std::string();
  }
}

std::string
Pathname::get_sys_path() const
{
  switch(m_type)
  {
    case kSysPath:
      return m_path;

    case kDataPath:
      if (s_datadir_overrides.empty())
      {
        return s_datadir + m_path;
      }
      else
      {
        Overrides::const_iterator it = s_datadir_overrides.find(m_path);
        if (it == s_datadir_overrides.end())
        {
          return s_datadir + m_path;
        }
        else
        {
          return it->second;
        }
      }

    case kUserPath:
      return s_userdir + m_path;

    case kEmpty:
      return "";

    default:
      assert(!"Never reached");
      return std::string();
  }
}

std::string
Pathname::get_extension() const
{
  for(int i = m_path.size()-1; i >= 0; --i)
  {
    if (m_path[i] == '.')
    {
      return m_path.substr(i+1);
    }
    else if (m_path[i] == '/')
    {
      return std::string();
    }
  }

  return std::string();
}

bool
Pathname::operator<(const Pathname& rhs) const
{
  return 
    m_path < rhs.m_path ||
    m_type < rhs.m_type;
}

std::ostream& operator<<(std::ostream& s, const Pathname& path)
{
  switch(path.get_type())
  {
    case Pathname::kSysPath:
      return s << "sys:" << path.get_raw_path() << " - " << path.get_sys_path();

    case Pathname::kDataPath:
      return s << "data:" << path.get_raw_path() << " - " << path.get_sys_path();

    case Pathname::kUserPath:
      return s << "user:" << path.get_raw_path() << " - " << path.get_sys_path();

    case Pathname::kEmpty:
      return s << "empty";

    default:
      assert(!"never reached");
      return s;
  }
}

/* EOF */
