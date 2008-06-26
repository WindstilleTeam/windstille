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
    out += tolower(*i);
  return out;
}

const std::wstring string_to_wstring(const std::string& str) {
	std::wstring tmp(str.begin(), str.end());
	return tmp;
}

const std::string wstring_to_string(const std::wstring& str) {
	std::string tmp(str.begin(), str.end());
	return tmp;
}

/* EOF */
