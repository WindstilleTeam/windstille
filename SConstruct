env = Environment()
env.ParseConfig("pkg-config --cflags --libs clanCore-1.0 clanApp-1.0 clanDisplay-1.0 clanGL-1.0")
env.ParseConfig("pkg-config --cflags --libs guile-1.8")
env.Append(LIBS=["GL", "GLU", "pthread"])
env.Program("windstille", Glob("src/*.cxx") + Glob("src/editor/*.cxx"))

# EOF #
