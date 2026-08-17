// miniswig - Language binding generator for Squirrel
// Copyright (C) 2005 Matthias Braun <matze@braunis.de>
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

#ifndef __CREATE_DOCU_H__
#define __CREATE_DOCU_H__

#include "tree.hpp"
#include "xmlwriter.hpp"

class DocuCreator
{
public:
    const char* ind;
    std::ostream& out;
    XmlWriter writer;

    DocuCreator(std::ostream& _out = std::cout) :
        ind("  "),
        out(_out),
        writer(out)
    { }

    void create_docu(Namespace* ns);
    void create_class_docu(Class* _class);
    void create_function_docu(Class* _class, Function* function);
    std::string get_type(const Type& type);

private:
    DocuCreator(const DocuCreator&);
    DocuCreator& operator=(const DocuCreator&);
};

#endif
