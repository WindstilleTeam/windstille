// miniswig - Language binding generator for Squirrel
// Copyright (C) 2005-2006 Matthias Braun <matze@braunis.de>
// Copyright (C) 2008 Ryan Flegel <rflegel@gmail.com>
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

#include <stdexcept>
#include <sstream>
#include "xmlwriter.hpp"

XmlWriter::XmlWriter(std::ostream& outstream) :
    out(outstream),
    indent(0),
    closetag(),
    lasttag(),
    sections()
{
}

XmlWriter::~XmlWriter()
{
    if(sections.size() > 0) {
        std::cerr << "WARNING: NOT CLOSED: ";
        for(std::vector<std::string>::iterator i = sections.begin();
                i != sections.end(); ++i)
            std::cerr << *i << " ";
        std::cerr << "\n";
    }

    closeTag();
}

void XmlWriter::openTag(const char* name)
{
    newLine();
    out << "<" << name;
    closetag = ">";
    indent++;

    sections.push_back(name);
}

void XmlWriter::closeTag(const char* name)
{
    if(sections.size() == 0)
	throw std::runtime_error("got closeSection without prior openSection.");

    const std::string& lastsection = sections.back();
    if (lastsection != name) {
        std::ostringstream msg;
        msg << "mismatch in open/closeSection. Expected '"
            << lastsection << "' got '" << name << "'";
        throw std::runtime_error(msg.str());
    }
    sections.pop_back();

    indent--;
    newLine();
    // XXX: We should check for consistency here
    out << "</" << name;
    closetag = ">" ;
}

void XmlWriter::writeTag(const char* name)
{
    newLine();
    out << "<" << name;
    closetag = "/>";
    lasttag = name;
}

void XmlWriter::newLine()
{
    if(closetag != "") {
        closeTag();
        for (int i=0;i<indent;i++)
            out << "\t";
    }
}

void XmlWriter::closeTag()
{
    if (closetag != "")
	out << closetag << "\n";
}
