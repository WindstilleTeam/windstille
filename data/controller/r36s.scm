;; -*- scheme -*-
;; R36S / ArkOS handheld (RK3326).
;;
;; When the pad is opened via SDL_GameController (preferred), the standard
;; button enum applies: A=0 B=1 X=2 Y=3 BACK=4 GUIDE=5 START=6
;; LEFTSTICK=7 RIGHTSTICK=8 LEFTSHOULDER=9 RIGHTSHOULDER=10
;; DPAD_UP=11 DPAD_DOWN=12 DPAD_LEFT=13 DPAD_RIGHT=14
;; Axes: LEFTX=0 LEFTY=1 RIGHTX=2 RIGHTY=3 …
;;
;; Menu navigation must use MENU_* buttons (not x/y axes). Bind D-pad to the
;; GameController DPAD buttons so the cursor can move.

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

 ;; D-pad (SDL_GameController DPAD_* → buttons 11–14)
 (menu-up-button    (joystick-button (device 0) (button 11)))
 (menu-down-button  (joystick-button (device 0) (button 12)))
 (menu-left-button  (joystick-button (device 0) (button 13)))
 (menu-right-button (joystick-button (device 0) (button 14)))
)

;; EOF ;;
