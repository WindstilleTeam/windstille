;; -*- scheme -*-
(windstille-controller
 (x-axis   (keyboard-axis (minus "Left") (plus "Right")))
 (y-axis   (keyboard-axis (minus "Up") (plus "Down")))
  
 (x2-axis  (keyboard-axis (minus "Keypad 4") (plus "Keypad 6")))
 (y2-axis  (keyboard-axis (minus "Keypad 8") (plus "Keypad 2")))

 (left-trigger-axis)
 (right-trigger-axis)

 (left-stick-button  (keyboard-button (key "X")))
 (right-stick-button (keyboard-button (key "Keypad 5")))

 (primary-button    (keyboard-button (key "S")))
 (secondary-button  (keyboard-button (key "D")))
 (tertiary-button   (keyboard-button (key "A")))
 (quaternary-button (keyboard-button (key "W")))

 (left-shoulder-button   (keyboard-button (key "Q")))
 (right-shoulder-button  (keyboard-button (key "E")))

 (select-button     (keyboard-button (key "R")))
 (start-button      (keyboard-button (key "F")))

 ;; don't bind those, since they only make sense on analog gamepads
 ;; with an additional dpad, on digital devices x-axs and y-axis
 ;; already serve the same purpose
 (menu-up-button)
 (menu-down-button)
 (menu-left-button)
 (menu-right-button)
 )

;; EOF ;;
