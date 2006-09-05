#ifndef __SPAWN_OBJECT_HPP__
#define __SPAWN_OBJECT_HPP__

#ifndef SCRIPTING_API
#include <squirrel.h>
#endif

namespace Scripting
{

/**
 * Spawn object. Parameters:
 *    name:    string with name of object
 *    table:   table that is parsed to get object properties
 *
 * Example: spawn_object("scriptable-object", {name="blup", pos=[350,370], sprite="images/arrow.sprite"});
 */
SQInteger spawn_object(HSQUIRRELVM v) __custom;

}

#endif

