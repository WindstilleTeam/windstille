#ifndef __SPRITE2D_MANAGER_HPP__
#define __SPRITE2D_MANAGER_HPP__

#include <string>
#include <map>
#include <boost/shared_ptr.hpp>

namespace sprite2d
{
typedef 

class Data;
typedef boost::shared_ptr<Data> DataPtr;

class Manager
{
public:
  Manager();
  ~Manager();

  DataPtr create_data(const std::string& filename);
  
  /** Removes all cached Sprites that are no longer in use */
  void cleanup();
private:
  typedef std::map<std::string, DataPtr> Datas;
  Datas datas;
};

}

extern sprite2d::Manager* sprite2d_manager;

#endif

