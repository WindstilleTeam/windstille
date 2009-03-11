function init()
{
  yagor <- Dialog(TOP, "Yagor", "images/portraits/yagor.sprite");
  jane  <- Dialog(TOP, "Jane", "images/portraits/jane.sprite");
}

function run()
{
  cutscene_begin();
  wait(1);

  yagor.show("How are you?<sleep>\nEverything fine today?");
  jane.show("Jep, thanks for asking. Buts its a bit dark here. Do you have a flashlight?");
  yagor.show("Sure, here take....");
  fadeout();
  wait_for_fade();
  fadein();

  yagor.show("Here it is.");
  jane.show("Ok, thanks, time to explore this apartment a bit.");

  objects.flashlight.set_active(true);
  objects.flashlight.set_parent("player");

  cutscene_end();
}

/* EOF */
