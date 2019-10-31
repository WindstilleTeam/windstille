;; Windstille - A Jump'n Shoot Game
;; Copyright (C) 2019 Ingo Ruhnke <grumbel@gmail.com>
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

(use-modules (ice-9 popen)
             (ice-9 rdelim)
             (guix build utils)
             (guix build-system scons)
             (guix git-download)
             (guix gexp)
             (guix licenses)
             (guix packages)
             (guix ui)
             ((gnu packages base) #:prefix base:)
             (gnu packages guile)
             (gnu packages linux)
             (gnu packages autotools)
             (gnu packages compression)
             (gnu packages gl)
             (gnu packages pkg-config)
             (gnu packages image)
             (gnu packages sdl)
             (gnu packages bdw-gc)
             (gnu packages swig)
             (gnu packages xiph)
             (gnu packages xorg))

(define %source-dir (dirname (current-filename)))

(define current-commit
  (with-directory-excursion %source-dir
                            (let* ((port   (open-input-pipe "git describe --tags"))
                                   (output (read-line port)))
                              (close-pipe port)
                              (string-trim-right output #\newline))))

(define clanlib-1.0 (load* (string-append %source-dir "/external/clanlib-1.0/guix.scm")
                           (make-user-module '())))

(define-public windstille-0.2
  (package
   (name "windstille-0.2")
   (version current-commit)
   (source
    (local-file %source-dir
                #:recursive? #t
                #:select? (git-predicate %source-dir)))
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
      ("guile-1.8" ,guile-1.8)
      ("libatomic-ops" ,libatomic-ops)
      ("clanlib-1.0" ,clanlib-1.0)))
   (synopsis "")
   (description "")
   (home-page "https://windstille.gitlab.io")
   (license gpl2+)))

windstille-0.2

;; EOF ;;
