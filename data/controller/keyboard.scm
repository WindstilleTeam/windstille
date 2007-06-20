;; -*- scheme -*-
(windstille-controller
 (x-axis   (keyboard-axis (minus "left") (plus "right")))
 (y-axis   (keyboard-axis (minus "up") (plus "down")))
  
 (x2-axis  (keyboard-axis (minus "[4]") (plus "[6]"))) 
 (y2-axis  (keyboard-axis (minus "[2]") (plus "[8]")))

 (view-center-button  (keyboard-button (key "[5]")))

 (primary-button      (keyboard-button (key "s")))
 (secondary-button    (keyboard-button (key "d")))
 (tertiary-button     (keyboard-button (key "a")))

 (pda-button          (keyboard-button (key "w")))
 (aim-button          (keyboard-button (key "k")))
 (pause-button        (keyboard-button (key "p")))
 (inventory-button    (keyboard-button (key "e")))

 ;; don't bind those, since they only make sense on analog gamepads
 ;; with an additional dpad, on digital devices x-axs and y-axis
 ;; already serve the same purpose
 (menu-up-button)
 (menu-down-button)
 (menu-left-button)
 (menu-right-button)
 )

;; EOF ;;
