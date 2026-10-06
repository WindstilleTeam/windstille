// Exercise free-function closures and .call() with an explicit environment.
function doit()
{
  ::print("doit() called\n");
}

// Invoke with an empty table as `this`
doit.call({});

/* EOF */
