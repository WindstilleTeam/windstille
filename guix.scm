;; Windstille - A Jump'n Shoot Game
;; Copyright (C) 2019,2020 Ingo Ruhnke <grumbel@gmail.com>
;;
;; This program is free software; you can redistribute it and/or
;; modify it under the terms of the GNU General Public License
;; as published by the Free Software Foundation; either version 2
;; of the License, or (at your option) any later version.
;;
;; This program is distributed in the hope that it will be useful,
;; but WITHOUT ANY WARRANTY; without even the implied warranty of
;; MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
;; GNU General Public License for more details.
;;
;; You should have received a copy of the GNU General Public License
;; along with this program; if not, write to the Free Software
;; Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.

(set! %load-path
  (cons* "/ipfs/QmeUZkZxWMHKNR4HAGU9G2VRFcyiSFt3JNpuF5RBC9wKZa/guix-cocfree_0.0.0-51-gc2bce55"
         %load-path))

(use-modules ((guix licenses) #:prefix license:)
             ((gnu packages base) #:prefix base:)
             (gnu packages bdw-gc)
             (gnu packages gl)
             (gnu packages guile)
             (gnu packages pkg-config)
             (gnu packages swig)
             (guix build-system scons)
             (guix packages)
             (guix-cocfree utils))

(define %source-dir (dirname (current-filename)))

(define clanlib-1.0
  (package-from-file
   "/ipfs/QmbZhaLZULD3EbKEdG4NnsVLgA9Jo2w7vt222aVryeWXkq/clanlib-1.0_1.0.0-18-gf0de815/guix.scm"))

(define-public windstille-0.2
  (package
   (name "windstille-0.2")
   (version (version-from-source %source-dir))
   (source (source-from-source %source-dir #:version version))
   (build-system scons-build-system)
   (arguments
    `(#:tests? #f  ; some swig dependency issue
      #:scons-flags (list (string-append "PREFIX=" %output))
       #:phases (modify-phases %standard-phases
                  (add-after 'unpack 'patch-paths
                    (lambda _
                      (substitute* "src/windstille_main.cxx"
                        (("\\.\\./data/") "../share/windstille-0.2/"))
                      #t)))))
   (native-inputs
    `(("pkg-config" ,pkg-config)))
   (inputs
    `(("mesa" ,mesa)
      ("which" ,base:which)
      ("glu" ,glu)
      ("swig" ,swig)
      ("guile-3.0" ,guile-3.0)
      ("libatomic-ops" ,libatomic-ops)
      ("clanlib-1.0" ,clanlib-1.0)))
   (synopsis (synopsis-from-source %source-dir))
   (description (description-from-source %source-dir))
   (home-page (homepage-from-source %source-dir))
   (license license:gpl2+)))

windstille-0.2

;; EOF ;;
