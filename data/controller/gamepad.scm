;; -*- scheme -*-
;; Generic SDL gamepad / handheld layout (Android, many PortMaster devices).
;; Button indices follow the common "Xbox-like" SDL mapping when the pad is
;; exposed as a joystick (not the GameController API).

(windstille-controller
 (x-axis  (joystick-axis (device 0) (axis 0)))
 (y-axis  (joystick-axis (device 0) (axis 1)))

 (x2-axis (joystick-axis (device 0) (axis 2)))
 (y2-axis (joystick-axis (device 0) (axis 3)))

 (left-trigger-axis)
 (right-trigger-axis)

 (left-stick-button  (joystick-button (device 0) (button 8)))
 (right-stick-button (joystick-button (device 0) (button 9)))

 (primary-button    (joystick-button (device 0) (button 0)))
 (secondary-button  (joystick-button (device 0) (button 1)))
 (tertiary-button   (joystick-button (device 0) (button 2)))
 (quaternary-button (joystick-button (device 0) (button 3)))

 (left-shoulder-button  (joystick-button (device 0) (button 4)))
 (right-shoulder-button (joystick-button (device 0) (button 5)))

 (select-button (joystick-button (device 0) (button 6)))
 (start-button  (joystick-button (device 0) (button 7)))
 (debug-button)

 ;; D-pad as hat axes when the driver exposes them as axes 6/7
 (menu-up-button    (joystick-axis-button (device 0) (axis 7) (up #t)))
 (menu-down-button  (joystick-axis-button (device 0) (axis 7) (up #f)))
 (menu-left-button  (joystick-axis-button (device 0) (axis 6) (up #t)))
 (menu-right-button (joystick-axis-button (device 0) (axis 6) (up #f)))
)

;; EOF ;;
