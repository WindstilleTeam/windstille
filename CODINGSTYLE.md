Windstille Style Guide
======================

**Note:** Not all current code follows these rules, but that might be
fixed in the future. Also don't follow these rules blindly, use your
brain in situation where divergence might provide an advantage.


Indention
---------

* indent with two spaces

* never use tabs

* the used indention style can be reproduced with (might vary in
  details):

   astyle --style=ansi -s2 -S

* always have a new line before an opening braked, i.e. write:

    if (foobar)
    {
     ...
    }

  not:

    if (foobar) {
      ...
    }


Naming
------

* s_ prefix for static variables

* g_ prefix for global variables

* m_ prefix for member variables

* class names are CamelCase

* variables are lower_case

* filenames are all lower_case


Includes
--------

* all #include directives are written relative to "src/", so write:

  #include "app/windstille_main.hpp"

  not:

  #include "windstille_main.hpp"
  #include "../app/windstille_main.hpp"
  ...


Exceptions
----------

* we use exceptions

* (almost) always write an else clause to an if that throws or returns, i.e.:

  if (error)
  {
    throw std::runtime_error(...);
  }
  else
  {
    // do something else
  }

* use std::runtime_error() and friends for errors


Pointer and Memory Allocation
-----------------------------

* use `std::unique_ptr<>` and `std::make_unique()`, never manually
  `delete` or `delete[]`

* prefer `std::unique_ptr<>` over `std::shared_ptr<>` when possible

* use RAII whenever possible

* if a class isn't supposed to be copied by value, make assignment and
  copy constructor private (maybe we should write our own class for
  that or use boost::noncopyable):

    private:
      YourClass (const YourClass&);
      YourClass& operator= (const YourClass&);


Warnings
--------

* all code must be at least -ansi -pedantic -Wall clean, we use
  -Werror for development


Other Stuff
-----------

* we do not use macros in places where a template or inline function would do

* we use float, not double

* always use C++ style casts, never use C style casts

* never write:

    int x, y;

  always use:

    int x;
    int y;

* end all files with an `/* EOF */` comment (use comment style
  appropriate to your language)

* commit early and often, do not wait till your changes have turned
  into a huge unverifiable mess, do not keep your work private

* the Google C++ Style Guide provides a good reference for style
  decisions not covered in this document:

  http://google-styleguide.googlecode.com/svn/trunk/cppguide.xml

