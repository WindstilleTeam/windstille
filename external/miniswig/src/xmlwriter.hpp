// miniswig - Language binding generator for Squirrel
// Copyright (C) 2005-2006 Matthias Braun <matze@braunis.de>
// Copyright (C) 2020 Ingo Ruhnke <grumbel@gmail.com>
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

#ifndef __XMLWRITER_H__
#define __XMLWRITER_H__

#include <iostream>
#include <vector>
#include <string>

/** This class is a class which helps printing formated xml output.
 * Example:
 *  This sequence:
 *   xml.openTag("world");
 *   xml.writeAttribute("name", "foo");
 *   xml.writeTag("bar");
 *   xml.writeTag("baz");
 *   xml.writeAttribute("name", "boo");
 *   xml.writeAttribute("style", "old");
 *   xml.write("text");
 *   xml.closeTag("world");
 *  results in this output:
 *   <world name="foo">
 *     <bar/>
 *     <baz name="boo" style="old">text</baz>
 *   </world>
 */
class XmlWriter {
public:
    XmlWriter(std::ostream& out);
    ~XmlWriter();

    /** Start a xml tag which contains subtags */
    void openTag(const char* name);
    /** Closes an xml tag with subtags */
    void closeTag(const char* name);

    void writeTag(const char* name);

    template <class T>
      void comment(const T& outp)
      {   // This routine writes just about anything as an XML comment.
	newLine();
	out << "<!-- " << outp ;
	closetag = " -->";
      }


    template<class T>
    void write(const T& text)
    {
        if (closetag[0]=='>') {
            out << ">";
            closetag = "";
        } else if (closetag[0]=='/') {
      	    out << ">"; // eventually we should place a \n here
	    closetag = "</";
	    closetag += lasttag;
	    closetag += ">";
	}
	out << text;
    }

    template<class T>
    void writeAttribute(const char* name, T value)
    {
	out << " " << name << "=\"" << value << "\"";
    }

private:
    void newLine();
    void closeTag();

    std::ostream& out;
    int indent;
    std::string closetag;
    std::string lasttag;
    std::vector<std::string> sections;
};

#endif
