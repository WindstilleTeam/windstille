#include <iostream>

int main(int argc, char** argv)
{
  if (argc == 1)
  {

  }
  else
  {
    for(int i = 1; i < argc; ++i)
    {
      Pathname pathname(argv[i], Pathname::kSysPath);
      std::cout << "syspath: " << pathname.get_sys_path() << std::endl;
      std::cout << "basename: " << pathname.get_basename() << std::endl;
      std::cout << "dirname:  " << pathname.get_dirname() << std::endl;
      std::cout << std::endl;
    }
  }

  return 0;
}

/* EOF */
