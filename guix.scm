;; Windstille - A Sci-Fi Action-Adventure Game
;; Copyright (C) 2019 Ingo Ruhnke <grumbel@gmail.com>
;;
;; This program is free software: you can redistribute it and/or modify
;; it under the terms of the GNU General Public License as published by
;; the Free Software Foundation, either version 3 of the License, or
;; (at your option) any later version.
;;
;; This program is distributed in the hope that it will be useful,
;; but WITHOUT ANY WARRANTY; without even the implied warranty of
;; MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
;; GNU General Public License for more details.
;;
;; You should have received a copy of the GNU General Public License
;; along with this program.  If not, see <http://www.gnu.org/licenses/>.

(use-modules (ice-9 popen)
             (ice-9 rdelim)
             (guix build utils)
             (guix build-system cmake)
             (guix build-system gnu)
             (guix download)
             (guix git-download)
             (guix gexp)
             ((guix licenses) #:prefix license:)
             (guix packages)
             (gnu packages audio)
             ((gnu packages base) #:prefix base:)
             (gnu packages autotools)
             (gnu packages boost)
             (gnu packages bison)
             (gnu packages flex)
             (gnu packages curl)
             (gnu packages fontutils)
             (gnu packages fribidi)
             (gnu packages game-development)
             (gnu packages gcc)
             (gnu packages gl)
             (gnu packages gtk)
             (gnu packages glib)
             (gnu packages image)
             (gnu packages pkg-config)
             (gnu packages python)
             (gnu packages sdl)
             (gnu packages squirrel)
             (gnu packages version-control)
             (gnu packages perl)
             (gnu packages netpbm)
             (gnu packages gettext)
             (gnu packages linux)
             (gnu packages maths)
             (gnu packages xiph)
             (gnu packages xorg))

(define %source-dir (dirname (current-filename)))

(define current-commit
  (with-directory-excursion %source-dir
                            (let* ((port   (open-input-pipe "git describe --tags"))
                                   (output (read-line port)))
                              (close-pipe port)
                              (string-trim-right output #\newline))))

(define-public xcftools-jjorge
  (let ((commit "9d9d0d3fc7546b0e5207562fed96893e3a591b11")
        (revision "0"))
    (package
     (name "xcftools-jjorge")
     (version (git-version "1.0.8" revision commit))
     (source
      (origin
       (method git-fetch)
       (uri (git-reference
             (url "https://github.com/Grumbel/xcftools.git")
             ;;(url "https://github.com/j-jorge/xcftools.git")
             (commit commit)))
       (file-name (git-file-name name version))
       (sha256
        (base32
         "11yjzahcldhznh0wsi968n59nh5skmn8c808bzl1h7gl19d1yb7i"))))
     (build-system gnu-build-system)
     (arguments
      `(#:tests? #f ; failing
        #:phases
        (modify-phases %standard-phases
                       (add-after 'unpack 'make-git-checkout-writable
                                  (lambda _
                                    (for-each make-file-writable (find-files "."))
                                    #t)))))
     (native-inputs
      `(("perl" ,perl)
        ("gettext" ,gettext-minimal)
        ("netpbm" ,netpbm)))
     (inputs
      `(("linux-headers" ,linux-libre-headers)
        ("libpng" ,libpng)))
     (synopsis "Command line tools for converting Gimp's .xcf files")
     (description "Xcftools is a set of fast command-line tools for
extracting information from the Gimp's native file format XCF.  The
tools are designed to allow efficient use of layered XCF files as
sources in a build system that use 'make' and similar tools to manage
automatic processing of the graphics.  These tools work independently
of the Gimp engine and do not require the Gimp to even be installed.

This version is a friendly fork of Henning Makholm's Xcftools with new
features.")
     (home-page "https://github.com/j-jorge/xcftools")
     (license license:gpl2+))))

(define-public gtkglext
  (package
   (name "gtkglext")
   (version "1.2.0")
   (source (origin
            (method url-fetch)
            (uri (string-append "mirror://sourceforge/gtkglext/gtkglext/" version "/"
                                name "-" version ".tar.gz"))
            (sha256
             (base32
              "1ya4d2j2aacr9ii5zj4ac95fjpdvlm2rg79mgnk7yvl1dcy3y1z5"))
            (modules '((guix build utils)))
            (snippet
             '(begin
                (substitute* "gdk/gdkglshapes.c"
                             ;; fix naming conflict with index(3)
                             (("index") "icosahedron_index"))
                (substitute* "gtk/gtkglwidget.c"
                             (("GTK_WIDGET_REALIZED") "gtk_widget_get_realized")
                             (("GTK_WIDGET_TOPLEVEL") "gtk_widget_is_toplevel")
                             (("!GTK_WIDGET_NO_WINDOW") "gtk_widget_get_has_window"))
                #t))))
   (build-system gnu-build-system)
   (native-inputs
    `(("pkg-config" ,pkg-config)))
   (propagated-inputs
    `(("pangox-compat" ,pangox-compat)
      ("mesa" ,mesa)
      ("glu" ,glu)))
   (inputs
    `(("gtk+-2" ,gtk+-2)
      ("glib:bin" ,glib "bin")         ; for glib-mkenums
      ("libx11"  ,libx11)
      ("libxt"  ,libxt)))
   (home-page "https://projects-old.gnome.org/gtkglext/")
   (synopsis "OpenGL extension to GTK+")
   (description "GtkGLExt is an OpenGL extension to GTK+.  It
provides additional GDK objects which support OpenGL rendering in GTK+
and GtkWidget API add-ons to make GTK+ widgets OpenGL-capable.  In
contrast to Janne Löf's GtkGLArea, GtkGLExt provides a GtkWidget API
that enables OpenGL drawing for standard and custom GTK+ widgets.")
   (license license:lgpl2.1+)))

(define-public gtkglextmm
  (package
   (name "gtkglextmm")
   (version "1.2.0")
   (source (origin
            (method url-fetch)
            (uri (string-append "mirror://sourceforge/gtkglext/gtkglextmm/" version "/"
                                name "-" version ".tar.gz"))
            (sha256
             (base32
              "0yyzbg3jicd4bww81i70hxxjdwnhq7r0ph88x765d337jlgrqjcg"))
            (modules '((guix build utils)))
            (snippet
             '(begin
                (substitute* "gdkglext/gdkmm/gl/Makefile.in"
                             (("-D[A-Z_]*_DISABLE_DEPRECATED") ""))
                (substitute* "gtkglext/gtkmm/gl/Makefile.in"
                             (("-D[A-Z_]*_DISABLE_DEPRECATED") ""))
                (substitute* "examples/Makefile.in"
                             (("-D[A-Z_]*_DISABLE_DEPRECATED") ""))
                #t))))
   (build-system gnu-build-system)
   (native-inputs
    `(("pkg-config" ,pkg-config)))
   (propagated-inputs
    `(("gtkglext" ,gtkglext)))
   (inputs
    `(("gtkmm-2" ,gtkmm-2)))
   (home-page "https://projects-old.gnome.org/gtkglext/")
   (synopsis "OpenGL extension to GTK+, C++ bindings")
   (description "GtkGLExt is an OpenGL extension to GTK+.  It
provides additional GDK objects which support OpenGL rendering in GTK+
and GtkWidget API add-ons to make GTK+ widgets OpenGL-capable.  In
contrast to Janne Löf's GtkGLArea, GtkGLExt provides a GtkWidget API
that enables OpenGL drawing for standard and custom GTK+ widgets.")
   (license license:lgpl2.1+)))

(define-public windstille
  (package
   (name "windstille")
   (version current-commit)
   (source (local-file %source-dir
                       #:recursive? #t
                       #:select? (git-predicate %source-dir)))
   (arguments
    `(#:tests? #f
      #:configure-flags '()))
   (build-system cmake-build-system)
   (native-inputs
    `(;("git" ,git)
      ("pkg-config" ,pkg-config)
      ("python" ,python)))
   (inputs
    `(("sdl2" ,sdl2)
      ("sdl2-image" ,sdl2-image)
      ("sdl2-mixer" ,sdl2-mixer)
      ("openal" ,openal)
      ("mesa" ,mesa)
      ("glew" ,glew)
      ("libvorbis" ,libvorbis)
      ("libogg" ,libogg)
      ("physfs" ,physfs)
      ("curl" ,curl)
      ("boost" ,boost)
      ("freetype" ,freetype)
      ("squirrel" ,squirrel)
      ("bison" ,bison)
      ("flex" ,flex)
      ("glm" ,glm)
      ("gtkmm-2" ,gtkmm-2)
      ("gtkglextmm" ,gtkglextmm)
      ("xcftools-jjorge" ,xcftools-jjorge)))
   (synopsis "2D platformer game")
   (description "Windstille is a free classic 2D jump'n run sidescroller game
in a style similar to the original Super Mario games covered under
the GNU GPL.")
   (home-page "https://windstille.org/")
   (license license:gpl3+)))

windstille

;; EOF ;;
