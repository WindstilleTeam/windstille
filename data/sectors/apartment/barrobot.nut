function init()
{
  println("Hello World: init");
}

function run()
{
  conversation_add2("Night Vision [On]", "A little dark here, can I get night vision.");
  conversation_add2("Night Vision [Off]", "This green look gets annoying.");

  switch (conversation_get())
    {
      case 0:
        nightvision_enabled(true)
        break;

      case 1:
        nightvision_enabled(false)
        break;
    }
}

/* EOF */
