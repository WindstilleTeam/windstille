/*
**  Windstille - A Sci-Fi Action-Adventure Game
**  Copyright (C) 2005 Matthias Braun <matze@braunis.de>
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

#include <fstream>

std::string dirname(const std::string& filename)
{
  std::string::size_type p = filename.find_last_of('/');
  if(p == std::string::npos)
    return "";

  return filename.substr(0, p+1);        
}

std::string basename(const std::string& filename)
{
  std::string::size_type p = filename.find_last_of('/');
  if(p == std::string::npos)
    return filename;

  return filename.substr(p+1, filename.size()-p);
}

void file_to_string(const std::string& filename, std::string& str)
{
  std::ifstream in(filename.c_str());
  while (true)
    {
      char c = static_cast<char>(in.get());
      if (!in.good())
        break;
      str += c;
    }
}

bool has_suffix(const std::string& str, const std::string& suffix)
{
  if (str.length() >= suffix.length())
    return str.compare(str.length() - suffix.length(), suffix.length(), suffix) == 0;
  else
    return false;
}

std::string tolowercase(const std::string& str)
{
  std::string out;
  for(std::string::const_iterator i = str.begin();  i != str.end(); ++i)
    out += static_cast<char>(tolower(*i));
  return out;
}

/* EOF */
