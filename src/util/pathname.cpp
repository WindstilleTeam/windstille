/*
**  Windstille - A Sci-Fi Action-Adventure Game
**  Copyright (C) 2009 Ingo Ruhnke <grumbel@gmail.com>
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

#include <assert.h>
#include <sstream>
#include <filesystem>

std::string Pathname::s_datadir;
std::string Pathname::s_userdir;
Pathname::Overrides Pathname::s_datadir_overrides;

void
Pathname::set_datadir_overrides(const Overrides& overrides)
{
  s_datadir_overrides = overrides;
}

static void add_overrides(const std::filesystem::path& path, const std::filesystem::path& base,
                          Pathname::Overrides* overrides)
{
  std::filesystem::directory_iterator end_i; // default construction yields past-the-end

  for(std::filesystem::directory_iterator i(path); i != end_i; ++i)
  {
    if (std::filesystem::is_directory(i->status()))
    {
      add_overrides(i->path(), base / i->path().filename(), overrides);
    }
    else
    {
      std::string from_path   = (base / i->path().filename()).string();
      std::string to_path     = i->path().string();
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

  if ( !s_datadir.empty() && s_datadir[s_datadir.size()-1] != '/' )
  {
    s_datadir += '/';
  }
}

void Pathname::set_userdir(const std::string& userdir)
{
  s_userdir = userdir;

  if ( !s_userdir.empty() && s_userdir[s_userdir.size()-1] != '/' )
  {
    s_userdir += '/';
  }
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
      return std::filesystem::exists(get_sys_path());
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
      assert(false && "Never reached");
      return std::string();
  }
}

std::string
Pathname::get_extension() const
{
  for(int i = static_cast<int>(m_path.size()) - 1; i >= 0; --i)
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
#pragma GCC diagnostic ignored "-Wzero-as-null-pointer-constant"
  return
    m_path < rhs.m_path ||
    m_type < rhs.m_type;
}

Pathname
Pathname::get_dirname() const
{
  std::string::size_type i = m_path.rfind('/');
  if (i == std::string::npos)
  {
    return Pathname("", m_type);
  }
  else
  {
    return Pathname(m_path.substr(0, i+1), m_type);
  }
}

Pathname
Pathname::get_basename() const
{
  std::string::size_type i = m_path.rfind('/');
  if (i == std::string::npos)
  {
    return *this;
  }
  else
  {
    return Pathname(m_path.substr(i+1), m_type);
  }
}

Pathname&
Pathname::append_text(const std::string& path)
{
  m_path += path;
  return *this;
}

Pathname&
Pathname::append_path(const std::string& path)
{
  if ( !m_path.empty() && m_path[m_path.size()-1] != '/' )
  {
    m_path += '/';
    m_path += path;
  }
  else
  {
    m_path += path;
  }

  return *this;
}

bool
Pathname::operator==(const Pathname& rhs) const
{
  return
    m_path == rhs.m_path &&
    m_type == rhs.m_type;
}

bool
Pathname::operator!=(const Pathname& rhs) const
{
  return
    m_path != rhs.m_path ||
    m_type != rhs.m_type;
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
      assert(false && "never reached");
      return s;
  }
}

/* EOF */
