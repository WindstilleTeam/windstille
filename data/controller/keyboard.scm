;; -*- scheme -*-
(windstille-controller
 (x-axis   (keyboard-axis (minus "left") (plus "right")))
 (y-axis   (keyboard-axis (minus "up") (plus "down")))
  
 (x2-axis  (keyboard-axis (minus "[4]") (plus "[6]"))) 
 (y2-axis  (keyboard-axis (minus "[8]") (plus "[2]")))

 (left-trigger-axis)
 (right-trigger-axis)

 (left-stick-button  (keyboard-button (key "x")))
 (right-stick-button (keyboard-button (key "[5]")))

 (primary-button    (keyboard-button (key "s")))
 (secondary-button  (keyboard-button (key "d")))
 (tertiary-button   (keyboard-button (key "a")))
 (quaternary-button (keyboard-button (key "w")))

 (left-shoulder-button   (keyboard-button (key "q")))
 (right-shoulder-button  (keyboard-button (key "e")))

 (select-button     (keyboard-button (key "r")))
 (start-button      (keyboard-button (key "f")))

 ;; don't bind those, since they only make sense on analog gamepads
 ;; with an additional dpad, on digital devices x-axs and y-axis
 ;; already serve the same purpose
 (menu-up-button)
 (menu-down-button)
 (menu-left-button)
 (menu-right-button)
 )

;; EOF ;;
