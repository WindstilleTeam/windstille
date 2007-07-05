;; -*- scheme -*-
;; Configuration for a Wiimote+Nunchuck which can be used with the cwiid library

(windstille-controller
 (x-axis  (wiimote-axis (device 0)  (axis 0)))
 (y-axis  (wiimote-axis (device 0)  (axis 1)))
 
 ;;(x2-axis (joystick-axis (device 0)  (axis 4)))
 ;;(y2-axis (joystick-axis (device 0)  (axis 3) (invert #t)))

 (view-center-button (wiimote-button (device 0) (button 9)))

 (primary-button    (wiimote-button (device 0)  (button 0)))
 (secondary-button  (wiimote-button (device 0)  (button 1)))
 (tertiary-button   (wiimote-button (device 0)  (button 11)))

 (pda-button        (wiimote-button (device 0)  (button 6)))

 (aim-button        (wiimote-button (device 0)  (button 12)))
 (pause-button      (wiimote-button (device 0)  (button 7)))
 (inventory-button  (wiimote-button (device 0)  (button 8)))

 ;; allow to use the dpad for menu navigation
 (menu-up-button    (wiimote-button (device 0)  (button 4)))
 (menu-down-button  (wiimote-button (device 0)  (button 5)))
 (menu-left-button  (wiimote-button (device 0)  (button 2)))
 (menu-right-button (wiimote-button (device 0)  (button 3)))
 )

;; EOF ;;
