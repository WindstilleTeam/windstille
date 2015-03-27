#include <iostream>

#include "util/baby_xml.hpp"

int main(int argc, char** argv)
{
  if (argc != 2)
  {
    std::cout << "Usage: " << argv[0] << " STRING" << std::endl;
  }
  else
  {
    // BabyXML xml("<strong size='10'>Hello &lt; World&gt; <b>&quot;blabla&quot;</b> Blablub</strong>");
    BabyXML xml(argv[1]);

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
        std::cout << " " << j->name << "='" << j->value << "'";
      }

      std::cout << std::endl;
    }
  }
}

/* EOF */
