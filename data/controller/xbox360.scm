;; -*- scheme -*-
;; Configuration for a XBox360 pad which can be used with the xpad driver

(windstille-controller
 (x-axis  (joystick-axis (device 0)  (axis 0)))
 (y-axis  (joystick-axis (device 0)  (axis 1)))
 
 (x2-axis (joystick-axis (device 0)  (axis 4)))
 (y2-axis (joystick-axis (device 0)  (axis 3) (invert #t)))

 (view-center-button (joystick-button (device 0)  (button 9)))

 (primary-button    (joystick-button (device 0)  (button 0)))
 (secondary-button  (joystick-button (device 0)  (button 1)))
 (tertiary-button   (joystick-button (device 0)  (button 3)))

 (pda-button        (joystick-button (device 0)  (button 2)))

 (aim-button        (joystick-button (device 0)  (button 5)))
 (pause-button      (joystick-button (device 0)  (button 6)))
 (inventory-button  (joystick-button (device 0)  (button 14)))

 ;; allow to use the dpad for menu navigation
 (menu-up-button    (joystick-button (device 0)  (button 10)))
 (menu-down-button  (joystick-button (device 0)  (button 11)))
 (menu-left-button  (joystick-button (device 0)  (button 12)))
 (menu-right-button (joystick-button (device 0)  (button 13)))
 )

;; EOF ;;
