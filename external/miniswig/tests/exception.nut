// Exception handling and cooperative threads (newthread / wakeup).

try {
  throw "Hello, World!";
} catch(err) {
  ::print("Caught exception: " + err + "\n");
}

class Exception
{
  message = "";

  constructor(msg)
  {
    message = msg;
  }
}

function my_thread()
{
  for (local i = 1; i < 4; ++i) {
    ::print("my_thread(): " + i + "\n");
    ::print("got: " + ::suspend(i) + "\n");
    if (i == 2) {
      throw Exception("exception from thread");
    }
  }
  return 0;
}

local thread = ::newthread(my_thread);

try {
  ::print("thread.call() -> " + thread.call() + "\n");
  while (thread.getstatus() == "suspended") {
    ::print("thread.wakeup() -> " + thread.wakeup(5) + "\n");
    ::print(thread.getstatus() + "\n");
  }
} catch(err) {
  if (err instanceof Exception) {
    ::print("EXCEPTION: " + err.message + "\n");
  } else {
    ::print("EXCEPTION: " + err + "\n");
  }
}

/* EOF */
