;; -*- scheme -*-
;; Generic SDL GameController layout (Android, PortMaster handhelds, R36S).
;; Button indices match SDL_GameControllerButton when the pad is opened via
;; the GameController API. Menu navigation uses the DPAD_* buttons (11–14).

(windstille-controller
 (x-axis  (joystick-axis (device 0) (axis 0)))
 (y-axis  (joystick-axis (device 0) (axis 1)))

 (x2-axis (joystick-axis (device 0) (axis 2)))
 (y2-axis (joystick-axis (device 0) (axis 3)))

 (left-trigger-axis  (joystick-axis (device 0) (axis 4)))
 (right-trigger-axis (joystick-axis (device 0) (axis 5)))

 (left-stick-button  (joystick-button (device 0) (button 7)))
 (right-stick-button (joystick-button (device 0) (button 8)))

 (primary-button    (joystick-button (device 0) (button 0)))
 (secondary-button  (joystick-button (device 0) (button 1)))
 (tertiary-button   (joystick-button (device 0) (button 2)))
 (quaternary-button (joystick-button (device 0) (button 3)))

 (left-shoulder-button  (joystick-button (device 0) (button 9)))
 (right-shoulder-button (joystick-button (device 0) (button 10)))

 (select-button (joystick-button (device 0) (button 4)))
 (start-button  (joystick-button (device 0) (button 6)))
 (debug-button  (joystick-button (device 0) (button 5)))

 ;; D-pad → menu navigation (required; menus ignore x/y axes)
 (menu-up-button    (joystick-button (device 0) (button 11)))
 (menu-down-button  (joystick-button (device 0) (button 12)))
 (menu-left-button  (joystick-button (device 0) (button 13)))
 (menu-right-button (joystick-button (device 0) (button 14)))
)

;; EOF ;;
