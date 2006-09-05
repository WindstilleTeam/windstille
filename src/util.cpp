#include "util.hpp"
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
      char c = in.get();
      if (!in.good())
        break;
      str += c;
    }
}

/* EOF */
