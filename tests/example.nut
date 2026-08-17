::print("example script\n");

do_foobar();
do_add(5, 10);
example <- Example();
example.do_foobar();
example.do_bazbaz();
do_custom(5, "test", 10);
do_suspend();
do_foobar();

assert(do_return_bool() == true);
assert(do_return_int() == 45);
assert(do_return_float() == 0.125);
assert(do_return_string() == "Hello World");

assert(do_return_bool_id(false) == false);
assert(do_return_int_id(99) == 99);
assert(do_return_float_id(0.0625) == 0.0625);
assert(do_return_string_id("Foobar") == "Foobar");

/* EOF */
