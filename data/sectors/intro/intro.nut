// -*- c++ -*-

function fade_in_out(r,g,b)
{
  fadeout(0.2, r,g,b);
  wait_for_fade();
  wait(1)
  caption_clear();
  fadein(3);
  wait_for_fade();
}

//render_mask_set(RENDER_MASK_COLORMAP);
fadeout(0);

objects.player.set_active(false);
spawn_object("scriptable-object", { name="planet", pos=[0,0],
                 sprite="images/planet.sprite", z_pos=1001 });

camera_set_active(false);
camera_set_pos(400, 300); // USA
camera_set_zoom(4.0);
fadein(1);
wait_for_fade();
cutscene_begin();
wait(2);

caption_add(BOTTOM, "Location: USA");
wait_for_dialog();

camera_continue_path();
camera_add_point(533, 248, 6.0); // greenland
camera_end_path();
wait_for_camera();

caption_add(BOTTOM, "Location: Greenland");
wait_for_dialog();

camera_continue_path();
camera_add_point(445, 513, 3.0); // antarctica
camera_end_path();
wait_for_camera();

caption_add(BOTTOM, "Location: Antarctica");
wait_for_dialog();

camera_add_point(400, 300, 1.0);
camera_add_point(400, 300, 1.0);
camera_end_path();
// wait_for_camera();

caption_add(BOTTOM, "The Big Boom. Humanity found out it could travel to the stars, and it did, en masse.");

wait_for_dialog();
fade_in_out(1,0,0)

caption_add(BOTTOM, "But it didn't take long for it to turn into The Big Bust. The guilds, the governments and the corporations staked out the best worlds.");

wait_for_dialog();
fade_in_out(1,1,1)

caption_add(BOTTOM, "Still, many individuals took their chances, even without military hardware and terraformers. Most worlds were so barren or so dangerous that whole colonies died out. In the Free Colonies that survive, life isn't easy.");

wait_for_dialog();

caption_end();

fadeout(1);
wait_for_fade();
objects.player.set_active(true);
objects.planet.remove();

cutscene_end();
camera_set_active(true);

render_mask_set(RENDER_MASK_ALL);
wait(1);
wait_for_fade();
fadein(1);


/* EOF */
