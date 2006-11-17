# -------------------------------------------------------
# Command line arguments
# -------------------------------------------------------
env = Environment()

mode = ARGUMENTS.get('mode', 'release').lower()  
platform = ARGUMENTS.get('os', str (Platform()))

print "platform = ", platform
print "mode     = ", mode

env.Replace ( PLATFORM = platform )
env.Replace ( MODE = mode )


#if (platform=='posix'):
#  env.Replace ( CXX = 'g++-3.3' )


buildroot = 'build' + '/' + platform + '/' + mode
buildmain = buildroot + '/main'
buildtest = buildroot + '/test'

resourcesroot = 'resources'
resourcesmain = resourcesroot + '/main'
resourcestest = resourcesroot + '/test'

env.Replace ( BUILDROOT = '#' + buildroot )



# -------------------------------------------------------
# Common methods
# -------------------------------------------------------
import os, fnmatch

# Allows to glob files from the build directory going
# through scons nodes
def Glob( env, pattern = '*.*', excludes = [], dir = '.' ):
    files = []
    for file in os.listdir( Dir(dir).srcnode().abspath ):
        if fnmatch.fnmatch(file, pattern) :
            excluded = False
            for xpattern in excludes :
              if fnmatch.fnmatch(file, xpattern) :
                excluded = True

            if excluded == False :
              files.append( os.path.join( dir, file ) )

    return files


def DefaultModuleName ( env, dir = '.' ):
    return os.path.basename (os.path.abspath (dir))

def DefaultTestModuleName ( env, dir = '.' ):
    return os.path.basename (os.path.abspath (dir) + '_test')


def DefineDefaultLibPath (env):
    platform = env['PLATFORM']
    librepo = ARGUMENTS.get('libs_repo_home', os.environ['LIBS_REPO_HOME'])   

    if (platform=='win32'):
        env.Prepend ( CPPPATH = [librepo + '/' + 'include'] )
        env.Prepend ( LIBPATH = [librepo + '/' + 'lib'] )
    elif (platform=='darwin'):
        env.Prepend ( CPPPATH = [librepo + '/' + 'include'] )
        env.Prepend ( LIBPATH = [librepo + '/' + 'lib'] )
    elif (platform=='posix'):
        env.Prepend ( CPPPATH = [librepo + '/' + 'include'] )
        env.Prepend ( LIBPATH = [librepo + '/' + 'lib'] )

    
def DefineDefaultCPPDefines (env):
    platform = env['PLATFORM']
    mode = env['MODE']

    if (platform=='posix') or (platform=='darwin'):
        env.Append ( CPPDEFINES = ['UNIX'] )
        if (mode=='debug'):
            env.Append ( CPPDEFINES = ['DEBUG'] )
    
    elif (platform=='win32'):
        env.Append ( CPPDEFINES = ['_MBCS', '_CONSOLE', '_USE_MATH_DEFINES', '__WIN32__', 'WIN32', '_AFXDLL' ] )
        if (mode=='debug'):
            env.Append ( CPPDEFINES = ['DEBUG', '_DEBUG'] )
        else:  
            env.Append ( CPPDEFINES = ['NDEBUG'] )


def DefineDefaultCCFlags (env):
    platform = env['PLATFORM']
    mode = env['MODE']

    if (platform=='posix'):
        if (mode=='debug'):
            env.Append ( CCFLAGS = ['-ggdb', '-fno-inline', '-fpermissive'] )
        else:
            env.Append ( CCFLAGS = ['-O3', '-fno-inline', '-fpermissive'] )

    elif (platform=='darwin'):
        if (mode=='debug'):
            env.Append ( CCFLAGS = ['-ggdb', '-fno-inline'] )
        else:
            env.Append ( CCFLAGS = ['-O3', '-fno-inline', '-fno-strength-reduce'] )


    elif (platform=='win32'):
        env.Append ( CCFLAGS = ['/EHsc', '/GR', '/GS', '/W3', '/nologo', '/c', '/Wp64', '/TP'] )

        if (mode=='debug'):
            env.Append ( CCFLAGS = ['/Od', '/MDd', '/Gm', '/RTC1', '/ZI'] )
        else:  
            env.Append ( CCFLAGS = ['/Ox', '/FD', '/MD', '/Zi'] )
    
  
def DefineDefaultLinkFlags (env):
    platform = env['PLATFORM']
    mode = env['MODE']

    if (platform=='win32'):
        env.Append ( LINKFLAGS = ['/INCREMENTAL:NO', '/NOLOGO', '/MACHINE:X86', '/SUBSYSTEM:CONSOLE', '/OPT:NOREF'] )
        if (mode=='debug'):
            env.Append ( LINKFLAGS = ['/DEBUG', '/NODEFAULTLIB:msvcrt.lib'] )
        else:  
            env.Append ( LINKFLAGS = ['/NODEFAULTLIB:msvcrtd.lib'] )



def DefineDefaultLibs (env):
    platform = env['PLATFORM']
    mode = env['MODE']

    if (platform=='posix'):
        # By default, everything has to be linked dynamically on posix
        # Too many problems with static links!
        env.Append ( LIBS = ['dl'] )

    if (platform=='win32'):
        env.Append ( LIBS = ['advapi32.lib'] )  
        if (mode=='debug'):
            env.Append ( LIBS = ['msvcrtd.lib'] )  
        else:  
            env.Append ( LIBS = ['msvcrt.lib'] )  



def AddBoostDependency (env, libname):
    platform = env['PLATFORM']
    mode = env['MODE']

    if platform == 'win32':
        # automatic link, no need to add libs
        return

    boostlib = libname + '-gcc'
    # always multithreaded by now
    boostlib = boostlib + "-mt"
    if (mode == 'debug'):
        boostlib = boostlib + "-d"

    env.Append (LIBS = [boostlib] )
    

def AddSQLiteDependency (env):
    platform = env['PLATFORM']
    mode = env['MODE']

    env.Append (LIBS = ['sqlite3'] )

    


def AppendMultithreadConf (env):
    platform = env['PLATFORM']
    mode = env['MODE']

    if (platform == 'posix'):
        env.Append (CCFLAGS= '-pthread' )
        env.Append (LIBS= ['pthread'] ) 

    if (platform == 'darwin'):
        env.Append (CCFLAGS= '-pthread' )
        env.Append (LIBS= ['pthread'] ) 

    elif (platform == 'win32'):
        env.Append ( LIBS = ['ws2_32.lib'] )  





def AddModuleDependency (env, modulename):
    env.Append ( MODULES = [modulename] )  
    env.Append ( LIBPATH = ['../../main/' + modulename] )  
    env.Append ( LIBS = [modulename] )  






def ModuleEnv (env):
    moduleenv = env.Copy()
    # Add default include path for dependencies
    moduleenv.Append (CPPPATH = [Dir('../../main').srcnode().abspath] )
    return moduleenv



    
def TestModuleEnv (env, includes='*.cpp', excludes=[], modules=[], boostlibs=[], withSQLite=False, withMultithreading=True):
    testmoduleenv = ModuleEnv (env)
    testmodulename = testmoduleenv.DefaultTestModuleName ()

    for module in modules:
      testmoduleenv.AddModuleDependency (module)

    for boostlib in boostlibs:
      testmoduleenv.AddBoostDependency (boostlib)

    if withSQLite == True:
      testmoduleenv.AddSQLiteDependency ()
      
    testmoduleenv.AppendMultithreadConf ();

    files = testmoduleenv.Glob (includes, excludes)
    testprogram = testmoduleenv.Program (testmodulename, files)
    testmoduleenv.Test("test.passed", testprogram)

    testmoduleresources = testmoduleenv.Dir ('resources').abspath
    testmoduleenv.AddPreAction ("test.passed",
                                [Delete (testmoduleresources),
                                 Copy (testmoduleresources, resourcestest)])
    testmoduleenv.AlwaysBuild ("test.passed")





# Declare common methods
from SCons.Script.SConscript import SConsEnvironment
SConsEnvironment.Glob=Glob
SConsEnvironment.DefaultModuleName=DefaultModuleName
SConsEnvironment.DefaultTestModuleName=DefaultTestModuleName
SConsEnvironment.DefineDefaultLibPath=DefineDefaultLibPath
SConsEnvironment.DefineDefaultCPPDefines=DefineDefaultCPPDefines
SConsEnvironment.DefineDefaultCCFlags=DefineDefaultCCFlags
SConsEnvironment.DefineDefaultLinkFlags=DefineDefaultLinkFlags
SConsEnvironment.DefineDefaultLibs=DefineDefaultLibs
SConsEnvironment.AppendMultithreadConf=AppendMultithreadConf
SConsEnvironment.ModuleEnv=ModuleEnv
SConsEnvironment.TestModuleEnv=TestModuleEnv
SConsEnvironment.AddModuleDependency=AddModuleDependency
SConsEnvironment.AddBoostDependency=AddBoostDependency
SConsEnvironment.AddSQLiteDependency=AddSQLiteDependency





# -------------------------------------------------------
# Common build configuration
# -------------------------------------------------------

env.DefineDefaultLibPath ()
env.DefineDefaultCPPDefines ()
env.DefineDefaultCCFlags ()
env.DefineDefaultLinkFlags ()
env.DefineDefaultLibs ()



# -------------------------------------------------------
# Custom builders
# -------------------------------------------------------
def builder_unit_test(target, source, env):
  app = str(source[0].abspath)

  # Exec in the test program dir!
  curdir = os.path.abspath (os.curdir)
  os.chdir (os.path.dirname (source[0].abspath))

  if os.system(app)==0:
    os.chdir (curdir)
    open(str(target[0]),'w').write("PASSED\n")
  else:
    os.chdir (curdir)
    return 1


def builder_copy_resources (target, source, env):
  env.Copy (target, source)


bld = Builder(action = builder_unit_test)
env.Append(BUILDERS = {'Test' :  bld})

bld = Builder(action = builder_copy_resources)
env.Append(BUILDERS = {'CopyResources' :  bld})








# -------------------------------------------------------
# Build process
# -------------------------------------------------------

Export('env')

builddir = '#build' + '/' + platform + '/' + mode



if 'doc' in COMMAND_LINE_TARGETS:
  env.SConscript ('doc/SConscript')
else:
  env.SConscript ('src/main/SConscript', build_dir = builddir + '/main', duplicate = 0)
  env.SConscript ('src/test/SConscript', build_dir = builddir + '/test', duplicate = 0)


    
    











