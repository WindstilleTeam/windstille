// miniswig - Language binding generator for Squirrel
// Copyright (C) 2005-2006 Matthias Braun <matze@braunis.de>
// Copyright (C) 2009-2020 Ingo Ruhnke <grumbel@gmail.com>
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <https://www.gnu.org/licenses/>.

#ifndef __CREATE_WRAPPER_H__
#define __CREATE_WRAPPER_H__

#include "tree.hpp"

class WrapperCreator
{
public:
    /// this is used for indentation
    const char* ind;
    // output stream
    std::ostream& out;
    std::ostream& hppout;

    WrapperCreator(std::ostream& _out = std::cout, std::ostream& _hppout = std::cout) :
        ind("  "),
        out(_out),
        hppout(_hppout),
        ns_prefix()
    { }

    void create_wrapper(Namespace* ns);

private:
    std::string ns_prefix;

    void create_register_functions_code(Namespace* ns);
    void create_register_function_code(Function* function, Class* _class);
    void create_register_classes_code(Namespace* ns);
    void create_register_class_code(Class* _class);
    void create_register_constant_code(Field* field);
    void create_register_constants_code(Namespace* ns);
    void create_register_slot_code(const std::string& what,
                                   const std::string& name);

    void create_function_list(Namespace* ns);
    void create_const_lists(Namespace* ns);
    void create_class_const_lists(Class* _class);
    void create_class_wrapper(Class* _class);
    void create_class_release_hook(Class* _class);
    void create_squirrel_instance(Class* _class);
    void create_function_wrapper(Class* _class, Function* function);
    void prepare_argument(const Type& type, size_t idx, const std::string& var);
    void push_to_stack(const Type& type, const std::string& var);

private:
    WrapperCreator(const WrapperCreator&);
    WrapperCreator& operator=(const WrapperCreator&);
};

#endif
