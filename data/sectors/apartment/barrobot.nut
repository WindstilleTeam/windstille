function init()
{
  println("Hello World: init");
}

function run()
{
  conversation_add2("Night Vision [On]", "A little dark here, can I get night vision.");
  conversation_add2("Lights [Off]", "Lets make it dark before playing with nightvision.");
  conversation_add2("Night Vision [Off]", "This green look gets annoying.");
  conversation_add2("Lights [On]", "Let there be light.");

  switch (conversation_get())
    {
      case 0:
        nightvision_enabled(true)
        break;

      case 2:
        nightvision_enabled(false)
        break;

      case 1:
        wait(0.4)
        objects.lamp1.set_active(false);
        wait(0.4)
        objects.lamp2.set_active(false);
        wait(0.4)
        objects.lamp3.set_active(false);
        break;

      case 3:
        wait(0.4)
        objects.lamp3.set_active(true);
        wait(0.4)
        objects.lamp2.set_active(true);
        wait(0.4)
        objects.lamp1.set_active(true);
        break;
    }
}

/* EOF */
