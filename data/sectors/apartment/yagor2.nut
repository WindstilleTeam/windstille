function init()
{
  println("barrobot.nut: init()");

  barrobot <- Dialog(TOP, "Yagor", "images/portraits/yagor.sprite");
  jane     <- Dialog(TOP, "Jane", "images/portraits/jane.sprite"); 
}

function run()
{
  println("barrobot.nut: run()");

  jane.say("So, what do you have to offer?");
  barrobot.say("Beer, plenty of bear, the finest brew in all of Nugani");
  jane.say("Anything else?");
  barrobot.say("Wiskey, Milk all that stuff, what do you want?");

  conversation_add2("Milk",    "Haven't drank any milk in a while.");
  conversation_add2("Wiskey",  "I don't think my stomach can take it.");
  conversation_add2("Beer",    "The old classic, always good, never great.");
  conversation_add2("Nothing", "Not thirsty.");

  switch (conversation_get())
    {
      case 0:
        jane.say("Give me a Milk.");
        barrobot.say("Milk drinker? Here you go.");
        break;

      case 1:
        jane.say("Give me a Wiskey, the best you have"); 
        barrobot.say("Here is the wiskey.");
        break;

      case 2:
        jane.say("Give me a beer, that finest of Nugani");
        barrobot.say("Here is the beer.");
        break;

      case 3:
        return;
        break;
    }

  return;

  conversation_add("Choice 1");
  conversation_add("Choice 2");
  conversation_get();

  conversation_add("Choice 1");
  conversation_add("Choice 2");
  conversation_add("Choice 3");
  conversation_get();

  conversation_add("Choice 1");
  conversation_add("Choice 2");
  conversation_add("Choice 3");
  conversation_add("Choice 4");
  conversation_get();

  conversation_add("Choice 1");
  conversation_add("Choice 2");
  conversation_add("Choice 3");
  conversation_add("Choice 4");
  conversation_add("Choice 5");
  conversation_get();

  conversation_add("Choice 1");
  conversation_add("Choice 2");
  conversation_add("Choice 3");
  conversation_add("Choice 4");
  conversation_add("Choice 5");
  conversation_add("Choice 6");
  conversation_get();

  conversation_add("Choice 1");
  conversation_add("Choice 2");
  conversation_add("Choice 3");
  conversation_add("Choice 4");
  conversation_add("Choice 5");
  conversation_add("Choice 6");
  conversation_add("Choice 7");
  conversation_get();

  conversation_add("Choice 1");
  conversation_add("Choice 2");
  conversation_add("Choice 3");
  conversation_add("Choice 4");
  conversation_add("Choice 5");
  conversation_add("Choice 6");
  conversation_add("Choice 7");
  conversation_add("Choice 8");
  conversation_get();

  conversation_add("Choice 1");
  conversation_add("Choice 2");
  conversation_add("Choice 3");
  conversation_add("Choice 4");
  conversation_add("Choice 5");
  conversation_add("Choice 6");
  conversation_add("Choice 7");
  conversation_add("Choice 8");
  conversation_add("Choice 9");
  conversation_get();
  
  conversation_add("Choice 1");
  conversation_add("Choice 2");
  conversation_add("Choice 3");
  conversation_add("Choice 4");
  conversation_add("Choice 5");
  conversation_add("Choice 6");
  conversation_add("Choice 7");
  conversation_add("Choice 8");
  conversation_add("Choice 9");
  conversation_add("Choice 10");
  conversation_get();
  
  // cutscene_begin();

  // barrobot <- Dialog(TOP, "Bar Robot", "images/portraits/barrobot.sprite");
  // jane  <- Dialog(TOP, "Jane", "images/portraits/jane.sprite");

  // barrobot.show("Hello how are you?")

  // // yagor.show("How are you?<sleep>\nEverything fine today?")
  // // jane.show("Jep, thanks for asking. Buts its a bit dark here. Do you have a flashlight?")
  // // yagor.show("Sure, here take....")
  // // fadeout()
  // // wait_for_fade()
  // // fadein();
  // // yagor.show("Here it is.")
  // // jane.show("Ok, thanks, time to explore this apartment a bit.")

  // // objects.flashlight.set_active(true);
  // // objects.flashlight.set_parent("player");

  // cutscene_end();
}

/* EOF */
