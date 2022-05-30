#include <iostream>

#include "util/pathname.hpp"
#include "util/directory.hpp"

using namespace windstille;

int main(int argc, char** argv)
{
  if (argc == 2)
  {
    Pathname directory_name(argv[1], Pathname::kSysPath);
    std::cout << "Directory: " << directory_name << std::endl;
    Directory::List const& directory = Directory::read(directory_name);
    for(Directory::List::const_iterator p = directory.begin(); p != directory.end(); ++p)
    {
      std::cout << "  Entry: " << *p << std::endl;
    }
  }
  else if (argc == 3)
  {
    Pathname directory_name(argv[1], Pathname::kSysPath);
    std::cout << "Directory: " << directory_name << std::endl;
    Directory::List const& directory = Directory::read(directory_name, argv[2]);
    for(Directory::List::const_iterator p = directory.begin(); p != directory.end(); ++p)
    {
      std::cout << "  Entry: " << *p << std::endl;
    }
  }
  else
  {
    std::cout << "Usage: " << argv[0] << " DIRECTORY [PATTERN]" << std::endl;
  }
}

/* EOF */
