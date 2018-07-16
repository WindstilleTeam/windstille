import os
env = Environment(ENV = os.environ)
env.ParseConfig("pkg-config --cflags --libs clanCore-1.0 clanApp-1.0 clanDisplay-1.0 clanGL-1.0 clanGUI-1.0 clanSound-1.0 clanVorbis-1.0 clanGUIStyleSilver-1.0")
env.ParseConfig("pkg-config --cflags --libs guile-1.8")
env.Append(LIBS=["GL", "GLU", "pthread"])
env.Append(CPPPATH=[".", "src/"])
env.Command("src/scripting/windstille_wrap.cxx",
            "src/scripting/windstille.i",
            "swig -guile -c++ -o $TARGET $SOURCE")
env.Program("windstille",
            Glob("src/*.cxx") +
            Glob("src/collision/*.cxx") +
            Glob("src/input/*.cxx") +
            Glob("src/gui/*.cxx") +
            Glob("src/guistyle/*.cxx") +
            Glob("src/scripting/*.cxx") +
            Glob("src/editor/*.cxx"))

# EOF #
