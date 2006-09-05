#ifndef __SPRITE3D_MANAGER_HPP__
#define __SPRITE3D_MANAGER_HPP__

#include <string>
#include <map>

namespace sprite3d
{

class Data;

class Manager
{
public:
  Manager();
  ~Manager();

  Data* create_data(const std::string& filename);

private:
  typedef std::map<std::string, Data*> Datas;
  Datas datas;
};

}

extern sprite3d::Manager* sprite3d_manager;

#endif

