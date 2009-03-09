;; -*- scheme -*-
;; Configuration for a XBox360 pad which can be used with the xpad driver

(windstille-controller
 (x-axis  (joystick-axis (device 0)  (axis 0)))
 (y-axis  (joystick-axis (device 0)  (axis 1)))
 
 (x2-axis (joystick-axis (device 0)  (axis 2)))
 (y2-axis (joystick-axis (device 0)  (axis 3)))

 (view-center-button (joystick-button (device 0)  (button 10)))

 (primary-button    (joystick-button (device 0)  (button 0)))
 (secondary-button  (joystick-button (device 0)  (button 1)))
 (tertiary-button   (joystick-button (device 0)  (button 2)))

 (pda-button        (joystick-button (device 0)  (button 3)))

 (aim-button        (joystick-button (device 0)  (button 5)))
 (pause-button      (joystick-button (device 0)  (button 7)))
 (inventory-button  (joystick-button (device 0)  (button 6)))

 ;; allow to use the dpad for menu navigation
 (menu-up-button    (joystick-axis-button (device 0) (axis 7) (up #t)))
 (menu-down-button  (joystick-axis-button (device 0) (axis 7) (up #f)))
 (menu-left-button  (joystick-axis-button (device 0) (axis 6) (up #t)))
 (menu-right-button (joystick-axis-button (device 0) (axis 6) (up #f)))
)

;; EOF ;;
