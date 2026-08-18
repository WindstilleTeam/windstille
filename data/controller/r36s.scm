;; -*- scheme -*-
;; R36S / ArkOS handheld (RK3326). Prefer launching via PortMaster so
;; gptokeyb / oga_controls can provide Select+Start exit; Windstille itself
;; only sees the SDL joystick device.
;;
;; Layout follows typical ArkOS SDL joystick indices for the built-in pad.
;; If a particular image differs, copy this file and adjust button numbers.

(windstille-controller
 (x-axis  (joystick-axis (device 0) (axis 0)))
 (y-axis  (joystick-axis (device 0) (axis 1)))

 (x2-axis (joystick-axis (device 0) (axis 2)))
 (y2-axis (joystick-axis (device 0) (axis 3)))

 (left-trigger-axis)
 (right-trigger-axis)

 (left-stick-button)
 (right-stick-button)

 ;; Face buttons (A/B/X/Y order varies by firmware; A=primary is common)
 (primary-button    (joystick-button (device 0) (button 0)))
 (secondary-button  (joystick-button (device 0) (button 1)))
 (tertiary-button   (joystick-button (device 0) (button 2)))
 (quaternary-button (joystick-button (device 0) (button 3)))

 (left-shoulder-button  (joystick-button (device 0) (button 4)))
 (right-shoulder-button (joystick-button (device 0) (button 5)))

 (select-button (joystick-button (device 0) (button 6)))
 (start-button  (joystick-button (device 0) (button 7)))
 (debug-button)

 ;; D-pad often mirrored on axes 0/1; also bind hat axes when present
 (menu-up-button    (joystick-axis-button (device 0) (axis 7) (up #t)))
 (menu-down-button  (joystick-axis-button (device 0) (axis 7) (up #f)))
 (menu-left-button  (joystick-axis-button (device 0) (axis 6) (up #t)))
 (menu-right-button (joystick-axis-button (device 0) (axis 6) (up #f)))
)

;; EOF ;;
