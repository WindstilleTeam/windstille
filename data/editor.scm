(use-modules (ice-9 pretty-print))

(load-from-path "helper.scm")

(define screen-width  (screen-get-width))
(define screen-height (screen-get-height))
(define empty (lambda () #f))
(define *tileeditor* #f)
(define *tileeditor-window* #f)
(define *tileselector-window* #f)
(define last-files (list *windstille-levelfile*))
(define datadir  *windstille-datadir*)

(define (push-last-file filename)
  (cond ((not (string=? filename (get-last-file)))
         (set! last-files (cons filename last-files)))))

(define (get-last-file)
  (car last-files))

(define (serialize-level)
  `(windstille-level
    (properties
     (name "Hello World")
     (width  ,(map-get-width))
     (height ,(map-get-height)))
    (scripts ,@(map-get-scripts))
    (tilemap
     (data ,@(map-get-data 1)))
    (background-tilemap
     (data ,@(map-get-data 0)))
    (diamond-map
     ,@(diamond-map-get-data)
     )))

(define (save-map filename)
  (let ((level (serialize-level)))
    (with-output-to-file filename
      (lambda ()
        (pretty-print level)
        (newline)))))

(gui-create-button-func 0 0
                        50 25 "New" 
                        (lambda () 
                          (show-new-level-dialog)))
(gui-create-button-func 0 25
                        50 25 "Load" 
                        (lambda ()
                          (simple-file-dialog "Load a level..." (get-last-file)
                                              (lambda (filename)
                                                (editor-load filename)
                                                (push-last-file filename)))))
(gui-create-button-func 0 50
                        50 25 "Save" 
                        (lambda () 
                          (simple-file-dialog "Save a level..." (get-last-file)
                                              (lambda (filename) 
                                                (save-map filename)
                                                (push-last-file filename)))))

(gui-create-button-func 0 75 50 25 "Play" 
                        (lambda ()
                          (let ((file (tmpnam)))
                            (save-map file)
                            (game-play file)
                            (delete-file file))))

(gui-create-button-func 0 100 50 25 "Quit" 
                        (lambda ()
                          (gui-quit)))

(gui-create-button-func 100 0
                        80 25 "Background" 
                        (lambda () (tilemap-set-active-layer 0)))

(gui-create-button-func 180 0
                        80 25 "Foreground" 
                        (lambda () (tilemap-set-active-layer 1)))

(gui-create-button-func 260 0
                        80 25 "Diamonds" 
                        (lambda ()
                          (tilemap-set-active-layer 1)))


(gui-create-button-func (- screen-width 80)
                        (- screen-height 25)
                        80 25 "Shell" 
                        windstille:repl)

(gui-create-button-func 0
                        (- screen-height 25)
                        100 25 "Tile" 
                        (lambda ()
                          (editor-set-tool 0)))

(gui-create-button-func (+ 100)
                        (- screen-height 25)
                        100 25 "Diamond" 
                        (lambda ()
                          (editor-set-tool 2)))

(gui-create-button-func (+ 200)
                        (- screen-height 25)
                        100 25 "Objects" 
                        (lambda ()
                          (editor-set-brush-tile 0)))

(gui-create-button-func (+ 300)
                        (- screen-height 25)
                        100 25 "Select"
                        (lambda ()
                          (editor-set-tool 1)))

(gui-create-button-func (+ 450)
                        (- screen-height 25)
                        80 25 "Tile Editor"
                        (lambda ()
                          (gui-component-toggle-visibility *tileeditor-window*)))

(gui-create-button-func (+ 570)
                        (- screen-height 25)
                        80 25 "Tile Selector"
                        (lambda ()
                          (gui-component-toggle-visibility *tileselector-window*)))

(define (show-new-level-dialog)
  (let ((window (gui-create-window 200 200 200 160 "Property Window")))
    (gui-push-component (gui-window-get-client-area window))

    (gui-create-label 10 10 "Width: ")
    (gui-create-label 10 30 "Height: ")

    (let ((width  (gui-create-inputbox 100 10 50 25 "50"))
          (height (gui-create-inputbox 100 30 50 25 "50"))

          (ok     (gui-create-button 90 100 50 25 "Ok"))
          (cancel (gui-create-button 140 100 50 25 "Cancel")))
      
      (gui-component-on-click ok 
                              (lambda ()   
                                (editor-new (string->number (gui-inputbox-get-text width))
                                            (string->number (gui-inputbox-get-text height)))
                                (gui-hide-component window)))

      (gui-component-on-click cancel
                              (lambda () 
                                (gui-hide-component window)))
      (gui-pop-component))))

(define (simple-file-dialog title filename func)
  (let ((window (gui-create-window 200 200 250 160 title)))
    (gui-push-component (gui-window-get-client-area window))
    (gui-create-label 10 10 "Filename: ")
    (let ((ok       (gui-create-button 90 100 50 25 "Ok"))
          (cancel   (gui-create-button 140 100 50 25 "Cancel"))
          (filename (gui-create-inputbox 10 30 180 30 filename))
          (browse   (gui-create-button 190 30 50 20 "Browse...")))

      (gui-component-on-click ok 
                              (lambda ()   
                                (func (gui-inputbox-get-text filename))
                                (gui-hide-component window)))

      (gui-component-on-click cancel
                              (lambda () 
                                (gui-hide-component window)))

      (gui-component-on-click browse
                              (lambda ()
                                (gui-file-dialog (gui-inputbox-get-text filename)
                                                 (lambda (filename)
                                                   (gui-inputbox-set-text filename)))))

      (gui-pop-component)
      window)))

(define (dump-tile-definitions filename)
  (with-output-to-file filename
    (lambda ()
      (pretty-print (get-tile-defs))
      (newline)
      (display ";; EOF ;;\n"))))


(let ((window (gui-create-window 200 200 250 180 "Tile Editor")))
  (gui-push-component (gui-window-get-client-area window))
  (set! *tileeditor* (editor-add-tileeditor 10 10))
  (let ((gettile (gui-create-button 148 10 75 25 "Get Tile"))
        (dump    (gui-create-button 148 95 75 25 "Dump")))
    
    (gui-component-on-click gettile
                            (lambda ()
                              (tileeditor-set-tile *tileeditor* (editor-get-brush-tile))))
    (gui-component-on-click dump
                            (lambda () 
                              (dump-tile-definitions (string-append datadir "tiles.scm"))))

    (gui-component-on-close window (lambda ()
                                     (gui-hide-component window)))

    (set! *tileeditor-window* window))
  (gui-pop-component))


(let ((window (gui-create-window 200 200 300 130 "Disclaimer")))
  (gui-push-component (gui-window-get-client-area window))
  (gui-create-label 10 10
                    (string-append "This editor is buggy and might crash quite a bit,\n"
                                   "it isn't really end user ready at the moment. \n"
                                   "It is included here for those who might find it usefull\n"
                                   "anyway, but don't complain when it locks your system"))
  (gui-create-button-func 210 70 75 25 "Ok"
                          (lambda ()
                            (gui-hide-component window)))
  (gui-pop-component))

(let ((window (gui-create-window 600 0 200 400 "TileSelector")))
  (gui-push-component (gui-window-get-client-area window))
  (tile-selector-create (- screen-width (* 3 64)) 0 3 8)
  (gui-component-on-close window (lambda ()
                                   (gui-hide-component window)))
  (set! *tileselector-window* window)
  (gui-pop-component))

(gui-hide-component *tileeditor-window*)

;; EOF ;;
