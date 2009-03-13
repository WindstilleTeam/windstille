function run() 
{
  // spawn_script("flicker.nut");

  println("spawn_function()");

  do_flicker <- false;

  if (false)
    { // Test of variable sharing
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
              do_flicker = !do_flicker;
            }
        });

      spawn_function(function() {
          while (true)
            {
              wait(0.1);

              if (::do_flicker)
                {
                  wait(0.1);
                  objects.lamp2.set_active(false);
                  wait(0.1);
                  objects.lamp2.set_active(true);
                  wait(0.1);
                  objects.lamp2.set_active(false);
                  wait(0.1);
                  objects.lamp2.set_active(true);
                  wait(1.0);
                }
            }
        });
    }
}

/* EOF */
