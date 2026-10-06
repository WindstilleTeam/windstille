// Broader language-feature exercise: table methods, varargs, classes,
// C++ suspend points, and exceptions.

function println(text)
{
  ::print(text + "\n");
}

// Function bound to an explicit environment table
glubtbl <- {};
function glub[glubtbl]()
{
  ::print("glub: env=" + glubtbl + " this=" + this + "\n");
}
function glubtbl::glub2()
{
  ::print("glub2: env=" + glubtbl + " this=" + this + "\n");
}
glub();
glubtbl.glub2();

// Table method with varargs
bar <- {};
function bar::foo(a, b, ...)
{
  ::print("foo: this=" + this + " a=" + a + " b=" + b + "\n");
  foreach (i, v in vargv) {
    ::print("  vargv[" + i + "] = " + v + "\n");
  }
}
bar.foo(5, 10, 15, 20);

// Lambda
local five = (@() 5)();
::print("lambda -> " + five + "\n");

// Cooperative suspend via the C++ binding (do_suspend)
do_suspend();
::print("resume 1\n");
do_suspend();
::print("resume 2\n");
do_suspend();
::print("resume 3\n");

// Classes
class Foobar
{
  a = 10;
  b = 5;
  static c = 10;

  constructor()
  {
    ::print("Foobar.constructor(): " + this + "\n");
    a += 10;
    b += 10;
  }

  function foo()
  {
    ::print("Foobar.foo(): " + a + " " + b + " " + c + "\n");
  }
}

local foobar = Foobar();
foobar.foo();

local foobar2 = Foobar();
foobar2.foo();

// Exceptions
::print("### Exception test\n");
try {
  throw 5;
} catch (e) {
  println("caught exception: " + e);
}

/* EOF */
