/*
**  Windstille - A Sci-Fi Action-Adventure Game
**  Copyright (C) 2005 Ingo Ruhnke <grumbel@gmx.de>
**
**  This program is free software: you can redistribute it and/or modify
**  it under the terms of the GNU General Public License as published by
**  the Free Software Foundation, either version 3 of the License, or
**  (at your option) any later version.
**  
**  This program is distributed in the hope that it will be useful,
**  but WITHOUT ANY WARRANTY; without even the implied warranty of
**  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
**  GNU General Public License for more details.
**  
**  You should have received a copy of the GNU General Public License
**  along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <sstream>
#include <stdexcept>
#include <iostream>
#include "baby_xml.hpp"

class BabyXMLReader
{
public:
  BabyXML* xml;
  std::string text;
  int cursor;
  int line_no;
  int column_no;
  
  BabyXMLReader(const std::string& text_, BabyXML* xml_) 
    : xml(xml_),
      text(text_),
      cursor(0),
      line_no(0),
      column_no(0)
  {
    std::string content;
    
    while(!eof())
      {
        char c = look_ahead();
        switch(c)
          {
          case '<':
            if (!content.empty())
              {
                xml->add_node(BabyXML::Node(BabyXML::Node::TEXT, content));
                content = "";
              }
            read_element();
            break;

          case '&':
            content += read_entity();
            break;

          default:
            content += c;
            incr_cursor();
            break;
          }
      }

    if (!content.empty())
      xml->add_node(BabyXML::Node(BabyXML::Node::TEXT, content));
  }

  void skip_space()
  {
    while(true) 
      {
        char c = look_ahead();
        if (c != ' ' && c != '\t') {
          break;
        } else {
          incr_cursor();
        }
      }
  }

  void read_char(char expect)
  {
    char c = look_ahead();
    if (c != expect)
      {
        std::ostringstream str;
        str << "Syntax Error:" << line_no << ":" << column_no 
            << ": Syntax error: read_char(): expected '" << expect << "' got '" << c << "'";
        throw std::runtime_error(str.str());
      }
    else
      {
        incr_cursor();
      }
  }

  void read_element()
  {
    BabyXML::Node node;

    read_char('<');
    if (look_ahead() == '/') 
      {
        node.type = BabyXML::Node::END_TAG;
        incr_cursor();
        node.content = read_identifer();
      }
    else
      {
        node.type = BabyXML::Node::START_TAG;
        node.content    = read_identifer();
        node.attributes = read_attributes();
      }
    
    skip_space();
    read_char('>');

    xml->add_node(node);
  }

  /** Read a string of the form: '"your string"' */
  std::string read_string()
  {
    char start = look_ahead();
    if (start != '\'' && start != '"')
      {
        std::ostringstream str;
        str << "Syntax Error:" << line_no << ":" << column_no 
            << ": Syntax error: read_string(): expected ' or \" got '" << start << "'";
        throw std::runtime_error(str.str());
      }
    incr_cursor();
    
    std::string content;
    while(true)
      {
        char c = look_ahead();
        if (c == '"' || c == '\'') 
          {
            if (c != start)
              {
                std::ostringstream str;
                str << "Syntax Error:" << line_no << ":" << column_no 
                    << ": Syntax error: read_string(): quote mismatch";
                throw std::runtime_error(str.str());
              }
            incr_cursor();
            break;
          } 
        else if (c == '&') 
          {
            c += read_entity();
          } 
        else 
          {
            incr_cursor();
            content += c;
          }
      }
    return content;
  }

  char read_entity()
  {
    read_char('&');
    std::string identifer = read_identifer();
    read_char(';');

    if (identifer == "quot")
      return '"';
    else if (identifer == "gt")
      return '>';
    else if (identifer == "lt")
      return '<';
    else
      {
        std::ostringstream str;
        str << "Syntax Error:" << line_no << ":" << column_no << ": Unknown entity: '&" + identifer + ";'";
        throw std::runtime_error(str.str());
      }
  }
  
  std::vector<BabyXML::Attribute> read_attributes()
  {
    std::vector<BabyXML::Attribute> attributes;
    while(true) 
      {
        skip_space();

        char c = look_ahead();
        if (c == '>' || c == '&' || c == '<' || c == ';')
          {
            break;
          }

        attributes.push_back(BabyXML::Attribute());

        attributes.back().name = read_identifer();
        read_char('=');
        attributes.back().value = read_string();
      }
    return attributes;
  }

  std::string read_identifer()
  {
    std::string identifer;
    while(true) 
      {
        char c = look_ahead();
        if (c == '<' || c == '>' || c == '&' || c == ';' || c == ' ' || c == '\t' || c == '=') {
          break;
        } else {
          identifer += c;
          incr_cursor();
        }
      }

    if (identifer.empty())
      {
        std::ostringstream str;
        str << "Syntax Error:" << line_no << ":" << column_no << ": Identifer is empty";
        throw std::runtime_error(str.str());
      }

    return identifer;
  }

  bool eof()
  {
    return cursor >= int(text.size());
  }

  char look_ahead()
  {
    if (cursor >= int(text.size()))
      {
        std::ostringstream str;
        str << "Syntax Error:" << line_no << ":" << column_no << ": Premature end of text";
        throw std::runtime_error(str.str());
      }

    return text[cursor];
  }

  void incr_cursor()
  {
    cursor += 1;
    if (text[cursor] == '\n')
      {
        line_no += 1;
        column_no = 0;
      }
    else
      {
        column_no += 1;
      }
  }
};

BabyXML::BabyXML(const std::string& text)
{
  BabyXMLReader(text, this);
}

BabyXML::~BabyXML()
{
  
}

void
BabyXML::add_node(const Node& node)
{
  nodes.push_back(node);
}

#ifdef TEST_BABY_XML
#include <iostream>

int main()
{
  // BabyXML xml("<strong size='10'>Hello &lt; World&gt; <b>&quot;blabla&quot;</b> Blablub</strong>");
  BabyXML xml("Hello ");

  for(BabyXML::iterator i = xml.begin(); i != xml.end(); ++i)
    {
      if (i->type == BabyXML::Node::START_TAG)
        std::cout << "STARTTAG: ";
      else if (i->type == BabyXML::Node::END_TAG)
        std::cout << "ENDTAG: ";
      else if (i->type == BabyXML::Node::TEXT)
        std::cout << "TEXT: ";

      std::cout << "'" << i->content << "'";

      for(std::vector<BabyXML::Attribute>::iterator j = i->attributes.begin(); j != i->attributes.end(); ++j)
        {
          std::cout << " " << j->name << "=" << j->value;
        }

      std::cout << std::endl;
    }
}
#endif

/* EOF */
