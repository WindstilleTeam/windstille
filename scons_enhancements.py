# Make all normal SConstruct calls available
from SCons.Script import *

def MakeEnvironment(packages, configuration):
    cfg = configuration.cfg
    env = Environment(ENV=os.environ)

    if configuration.host:
        env['CXX']    = configuration.host + "-g++"
        env['AR']     = configuration.host + "-ar"
        env['RANLIB'] = configuration.host + "-ranlib"
        env['LD']     = configuration.host + "-ld"

    for pkg in packages:
        # normal packgae name that has to be looked up in the configuration
        if type(pkg) == str:
            if pkg in cfg:
                env.Append(**cfg[pkg])
                # FIXME: hack
                if 'CXX' in cfg[pkg]:
                    env['CXX'] = cfg[pkg]['CXX']
                if 'AR' in cfg[pkg]:
                    env['AR'] = cfg[pkg]['AR']
                if 'RANLIB' in cfg[pkg]:
                    env['RANLIB'] = cfg[pkg]['RANLIB']                                        
            else:
                raise Exception("Unknown package: %s" % pkg)
            
        # 'instant package'
        elif type(pkg) == dict:
            env.Append(**pkg)

        # unknown stuff
        else:
            raise Exception("Unknown package type: %s" % pkg)

    return env

def ParseConfig(cmd):
    env = Environment()
    env['ENV']['PATH'] = os.environ['PATH']
    # env['ENV']['PKG_CONFIG_LIBDIR'] = os.environ['PKG_CONFIG_LIBDIR']
    # env['ENV']['PKG_CONFIG_PATH']   = os.environ['PKG_CONFIG_PATH']
    env.ParseConfig(cmd)

    cfg = {}

    for var in [ 'CCFLAGS', 'CPPDEFINES', 'CPPPATH', 'LIBS', 'LIBPATH' ]:
        if var in env and env[var] != []:
            cfg[var]  = env[var] 

    # if 'CCFLAGS' in cfg:
    #     cfg['CXXFLAGS'] = cfg['CCFLAGS']

    return cfg

def BuildProgram(target, sources, packages = []):
    return Configuration.cfg.BuildProgram(target, sources, packages)

def BuildStaticLibrary(target, sources, packages = []):
    return Configuration.cfg.BuildStaticLibrary(target, sources, packages)

class Configuration:
    cfg = {}
    
    def __init__(self, host = None):
        self.cfg = {}
        self.host = host
    
    def __getitem__(self, key):
        return self.cfg[key]

    def __setitem__(self, key, value):
        self.cfg[key] = value

    def __delitem__(self, key):
        del self.cfg[key]

    def Package(self, name, **keys):
        if name in self.cfg:
            raise SCons.Errors.UserError, "package %s already exists" % name
        else:
            if 'DEPENDS' in keys:
                del keys['DEPENDS']
            elif 'CMD' in keys:
                keys.update(ParseConfig(keys['CMD'])) # FIXME: wrong
                del keys['CMD']
                
            self.cfg[name] = keys
            
    def BuildProgram(self, target, sources, packages = []):
        env = MakeEnvironment(packages, self)
        return env.Program(target, sources)

    def BuildStaticLibrary(self, target, sources, packages = []):
        env = MakeEnvironment(packages, self)
        return env.StaticLibrary(target, sources)

    def debug_print(self):
        print
        print "Current Configuration:"
        print "======================"
        for k, v in self.cfg.items():
            print "cfg['%s'] = %s" % (k, v)
        print

# EOF #
