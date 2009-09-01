;; -*- scheme -*-
(windstille-controller
 (x-axis   (keyboard-axis (minus "left") (plus "right")))
 (y-axis   (keyboard-axis (minus "up") (plus "down")))
  
 (x2-axis  (keyboard-axis (minus "[4]") (plus "[6]"))) 
 (y2-axis  (keyboard-axis (minus "[8]") (plus "[2]")))

 (left-trigger-axis)
 (right-trigger-axis)

 (left-stick-button  (keyboard-button (key "q")))
 (right-stick-button (keyboard-button (key "[5]")))

 (primary-button    (keyboard-button (key "o")))
 (secondary-button  (keyboard-button (key "a")))
 (tertiary-button   (keyboard-button (key "e")))
 (quaternary-button (keyboard-button (key ",")))

 (left-shoulder-button   (keyboard-button (key "'")))
 (right-shoulder-button  (keyboard-button (key ".")))

 (select-button     (keyboard-button (key "p")))
 (start-button      (keyboard-button (key "u")))
 (debug-button      (keyboard-button (key "d")))

 ;; don't bind those, since they only make sense on analog gamepads
 ;; with an additional dpad, on digital devices x-axs and y-axis
 ;; already serve the same purpose
 (menu-up-button)
 (menu-down-button)
 (menu-left-button)
 (menu-right-button)
 )

;; EOF ;;
