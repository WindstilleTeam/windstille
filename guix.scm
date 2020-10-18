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

(set! %load-path
  (cons* "/ipfs/QmX6nVomBirTsHRVDiuQQgPsRWoS7HxcgFeAgwJWdGQ332/guix-cocfree_0.0.0-62-g3b27118"
         %load-path))

(use-modules (guix build-system cmake)
             ((guix licenses) #:prefix license:)
             (guix packages)
             (gnu packages audio)
             (gnu packages bison)
             (gnu packages curl)
             (gnu packages flex)
             (gnu packages fontutils)
             (gnu packages game-development)
             (gnu packages gcc)
             (gnu packages gettext)
             (gnu packages gl)
             (gnu packages glib)
             (gnu packages gtk)
             (gnu packages gnome)
             (gnu packages image)
             (gnu packages serialization)
             (gnu packages maths)
             (gnu packages netpbm)
             (gnu packages perl)
             (gnu packages pkg-config)
             (gnu packages pretty-print)
             (gnu packages sdl)
             (gnu packages squirrel)
             (gnu packages xiph)
             (guix-cocfree packages xcftools-jjorge)
             (guix-cocfree utils))

(define %source-dir (dirname (current-filename)))

(define-public windstille
  (package
   (name "windstille")
   (version (version-from-source %source-dir))
   (source (source-from-source %source-dir))
   (arguments
    `(#:tests? #f))
   (build-system cmake-build-system)
   (native-inputs
    `(("pkg-config" ,pkg-config)
      ("gcc" ,gcc-10)))
   (inputs
    `(("sdl2" ,sdl2)
      ("sdl2-image" ,sdl2-image)
      ("sdl2-mixer" ,sdl2-mixer)
      ("openal" ,openal)
      ("mesa" ,mesa)
      ("glew" ,glew)
      ("libvorbis" ,libvorbis)
      ("libogg" ,libogg)
      ("opusfile" ,opusfile)
      ("physfs" ,physfs)
      ("curl" ,curl)
      ("freetype" ,freetype)
      ("squirrel" ,squirrel)
      ("libsigc++" ,libsigc++)
      ("jsoncpp" ,jsoncpp)
      ("bison" ,bison)
      ("flex" ,flex)
      ("glm" ,glm)
      ("fmt" ,fmt)
      ("gtkmm" ,gtkmm)
      ("xcftools-jjorge" ,xcftools-jjorge)))
   (synopsis (synopsis-from-source %source-dir))
   (description (description-from-source %source-dir))
   (home-page (homepage-from-source %source-dir))
   (license license:gpl3+)))

windstille

;; EOF ;;
