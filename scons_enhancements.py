# Make all normal SConstruct calls available
from SCons.Script import *

def MakeEnvironment(packages, cfg):
    env = Environment(ENV=os.environ)

    for pkg in packages:
        if type(pkg) == str:
            if pkg in cfg:
                env.Append(**cfg[pkg])
            else:
                raise Exception("Unknown package: %s" % pkg)
        elif type(pkg) == dict:
            env.Append(**pkg)
        else:
            raise Exception("Unknown package type: %s" % pkg)

    return env

def BuildProgram(target, sources, packages = [], cfg = {}):
    env = MakeEnvironment(packages, cfg)
    return env.Program(target, sources)

def BuildStaticLibrary(target, sources, packages = [], cfg = {}):
    env = MakeEnvironment(packages, cfg)
    return env.StaticLibrary(target, sources)

def ParseConfig(cmd):
    env = Environment()
    env['ENV']['PATH'] = os.environ['PATH']
    env.ParseConfig(cmd)

    cfg = {}

    for var in [ 'CCFLAGS', 'CPPDEFINES', 'CPPPATH', 'LIBS', 'LIBPATH' ]:
        if var in env and env[var] != []:
            cfg[var]  = env[var] 

    # if 'CCFLAGS' in cfg:
    #     cfg['CXXFLAGS'] = cfg['CCFLAGS']

    return cfg

class Configuration:
    def __init__(self):
        self.cfg = {}
    
    def __getitem__(self, key):
        return self.cfg[key]

    def __setitem__(self, key, value):
        self.cfg[key] = value

    def __delitem__(self, key):
        del self.cfg[key]

    def Program(self, target, sources, packages = []):
        env = MakeEnvironment(packages, self.cfg)
        return env.Program(target, sources)

    def StaticLibrary(self, target, sources, packages = []):
        env = MakeEnvironment(packages, self.cfg)
        return env.StaticLibrary(target, sources)

    def debug_print(self):
        print
        print "Current Configuration:"
        print "======================"
        for k, v in self.cfg.items():
            print "cfg['%s'] = %s" % (k, v)
        print

# EOF #
