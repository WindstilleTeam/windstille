import os

PREFIX = ARGUMENTS.get('PREFIX', "/usr")

env = Environment(ENV = os.environ)
env["CXXFLAGS"] = ["-g", "-O2"]
env.ParseConfig("pkg-config --cflags --libs clanCore-1.0 clanApp-1.0 clanDisplay-1.0 clanGL-1.0 clanGUI-1.0 clanSound-1.0 clanVorbis-1.0 clanGUIStyleSilver-1.0")
env.ParseConfig("pkg-config --cflags --libs guile-3.0")
env.Append(LIBS=["GL", "GLU", "pthread"])
env.Append(CPPPATH=[".", "src/"])
env.Command("src/scripting/windstille_wrap.cxx",
            "src/scripting/windstille.i",
            "swig -guile -c++ -o $TARGET $SOURCE")
libcol = env.StaticLibrary("libcol", Glob("src/collision/*.cxx"))
env.Append(LIBS=[libcol])
env.Program("windstille",
            Glob("src/*.cxx") +
            Glob("src/collision/*.cxx") +
            Glob("src/input/*.cxx") +
            Glob("src/gui/*.cxx") +
            Glob("src/guistyle/*.cxx") +
            Glob("src/scripting/*.cxx") +
            Glob("src/editor/*.cxx"))

env.Program("colltest",
            ["test/colltest.cxx"])

env.Program("collision_benchmark",
            ["test/collision_benchmark.cxx"])

env.Program("collision_sprite_test",
            ["test/collision_sprite_test.cxx"])

# Install
prefix = PREFIX
bindir = os.path.join(prefix, "bin")
datadir = os.path.join(prefix, "share/windstille-0.2")
metainfo_dir = os.path.join(prefix, "share/metainfo")
applications_dir = os.path.join(prefix, "share/applications")
icon_dir = os.path.join(prefix, "share/icons/hicolor/scalable/apps")

env.InstallAs(os.path.join(bindir, "windstille-0.2"), "windstille")
for root, dirs, files in os.walk("data"):
    relroot = os.path.relpath(root, "data")
    for f in files:
        env.Install(os.path.join(datadir, relroot),
                    os.path.join(root, f))

env.Install(metainfo_dir, "windstille-0.2.appdata.xml")
env.Install(applications_dir, "windstille-0.2.desktop")
env.Install(icon_dir, "windstille-0.2.svg")

env.Alias('install', bindir)
env.Alias('install', datadir)
env.Alias('install', metainfo_dir)
env.Alias('install', applications_dir)
env.Alias('install', icon_dir)

# EOF #
