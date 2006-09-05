#ifndef __SERIALIZE_HPP__
#define __SERIALIZE_HPP__

#include <squirrel.h>
#include <string>

void save_squirrel_table(HSQUIRRELVM v, int table_idx, const std::string& file);
void load_squirrel_table(HSQUIRRELVM v, int table_idx, const std::string& file);

#endif


