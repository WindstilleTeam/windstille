function run() 
{
  // spawn_script("flicker.nut");

  println("spawn_function()");
  spawn_function(function() {
      while (true)
        {
          wait(0.1);
          objects.lamp3.set_active(false);
          wait(0.1);
          objects.lamp3.set_active(true);
          wait(0.1);
          objects.lamp3.set_active(false);
          wait(0.1);
          objects.lamp3.set_active(true);
          wait(1.0);
        }
    });
}

    /* EOF */
