;; -*- scheme -*-
(windstille-controller
 (x-axis
  (keyboard-axis (minus "left") (plus "right"))
  (joystick-axis (device 0)  (axis 0)))

 (y-axis
  (keyboard-axis (minus "up") (plus "down"))
  (joystick-axis (device 0)  (axis 1)))
 
 (x2-axis
  (keyboard-axis (minus "[4]") (plus "[6]"))
  (joystick-axis (device 0)  (axis 4)))

 (y2-axis
  (keyboard-axis (minus "[2]") (plus "[8]"))
  (joystick-axis (device 0)  (axis 3) (invert #t)))

 (view-center-button
  (keyboard-button (key "[5]"))
  (joystick-button (device 0)  (button 9)))

 (primary-button
  (keyboard-button (key "s"))
  (joystick-button (device 0)  (button 0)))

 (secondary-button
  (keyboard-button (key "d"))
  (joystick-button (device 0)  (button 1)))

 (tertiary-button
  (keyboard-button (key "a"))
  (joystick-button (device 0)  (button 3)))

 (pda-button
  (keyboard-button (key "w"))
  (joystick-button (device 0)  (button 2)))

 (aim-button
  (keyboard-button (key "k"))
  (joystick-button (device 0)  (button 5)))
 
 (pause-button
  (keyboard-button (key "p"))
  (joystick-button (device 0)  (button 6)))

 (inventory-button
  (keyboard-button (key "e"))
  (joystick-button (device 0)  (button 14)))

 (menu-up-button
  (joystick-button (device 0)  (button 10)))

 (menu-down-button
  (joystick-button (device 0)  (button 11)))

 (menu-left-button
  (joystick-button (device 0)  (button 12)))

 (menu-right-button
  (joystick-button (device 0)  (button 13)))
 )

;; EOF ;;
