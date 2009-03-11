function run()
{
  while (true)
    {
        wait(0.1)
        objects.lamp3.set_active(false);
        wait(0.1)
        objects.lamp3.set_active(true);
        wait(0.1)
        objects.lamp3.set_active(false);
        wait(0.1)
        objects.lamp3.set_active(true);
        wait(1.0)
    }
}
