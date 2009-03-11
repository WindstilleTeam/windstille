;; -*- scheme -*-
;; Configuration for a XBox360 pad which can be used with the xpad driver

(windstille-controller
 (x-axis  (joystick-axis (device 0)  (axis 0)))
 (y-axis  (joystick-axis (device 0)  (axis 1)))
 
 (x2-axis (joystick-axis (device 0)  (axis 2)))
 (y2-axis (joystick-axis (device 0)  (axis 3)))

 (left-trigger-axis   (joystick-axis (device 0)  (axis 5)))
 (right-trigger-axis  (joystick-axis (device 0)  (axis 4)))

 (left-stick-button  (joystick-button (device 0)  (button  9)))
 (right-stick-button (joystick-button (device 0)  (button 10)))

 (primary-button    (joystick-button (device 0)  (button 0)))
 (secondary-button  (joystick-button (device 0)  (button 1)))
 (tertiary-button   (joystick-button (device 0)  (button 2)))
 (quaternary-button (joystick-button (device 0)  (button 3)))

 (left-shoulder-button    (joystick-button (device 0)  (button 4)))
 (right-shoulder-button   (joystick-button (device 0)  (button 5)))

 (select-button     (joystick-button (device 0)  (button 6)))
 (start-button      (joystick-button (device 0)  (button 7)))

 ;; allow to use the dpad for menu navigation
 (menu-up-button    (joystick-axis-button (device 0) (axis 7) (up #t)))
 (menu-down-button  (joystick-axis-button (device 0) (axis 7) (up #f)))
 (menu-left-button  (joystick-axis-button (device 0) (axis 6) (up #t)))
 (menu-right-button (joystick-axis-button (device 0) (axis 6) (up #f)))
)

;; EOF ;;
