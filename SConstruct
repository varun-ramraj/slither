# SCons declarative input file that controls how Slither is built...

# Imports...
import os, sys, platform

# Grab environment object and prepare prettier build messages......
env = Environment(CXXCOMSTR     = "Compiling $SOURCE ...",
                  LINKCOMSTR    = "Linking $TARGET ...",
                  ARCOMSTR      = "Archiving $TARGET ...",
                  RANLIBCOMSTR  = "Indexing $TARGET ...")

# SCons internal settings...
env.SourceSignatures('MD5')

# Versioning...
env.VERSION_MAJOR   = 0
env.VERSION_MINOR   = 45
env.VERSION_SVN     = os.popen('svnversion .').read()[:-1]
env.VERSION_SVN     = env.VERSION_SVN.split(':')[-1]

# Common defines...
env.Append(CPPDEFINES=[('VERSION_MAJOR', env.VERSION_MAJOR),
                       ('VERSION_MINOR', env.VERSION_MINOR),
                       ('VERSION_SVN', "'wxT(\"" + env.VERSION_SVN + "\")'")])

# Prepare compiler include and linker flags for wxWidgets...
DebugToggle = ARGUMENTS.get('debug', 1)
if int(DebugToggle):
    env.ParseConfig('wx-config --cxxflags --libs std,core,base,media --debug=yes'
                    ' --unicode=yes')
    env.Append(CXXFLAGS = '-g3 -O0 -Wall -Werror')
else:
    env.ParseConfig('wx-config --cxxflags --libs std,core,base,media --debug=no'
                    ' --unicode=yes')
    env.Append(CXXFLAGS = '-O3')

# Add some additional search paths. Add more as necessary for your system...
env.Append(CPPPATH = os.popen('echo $HOME').read()[:-1] + str("/local/include"))

# Prepare linker flags for OS X stuff manually, since Apple violated the FHS...
if sys.platform == 'darwin':
    env.MergeFlags(env.ParseFlags("-isysroot /Developer/SDKs/MacOSX10.5.sdk " + \
                   "-framework QuartzCore "))

# Slither...
SlitherProgram = env.Program(
    'slither', ['src/AnalysisThread.cpp',
                'src/CaptureThread.cpp',
                'src/Experiment.cpp',
                'src/ImageAnalysisWindow.cpp',
                'src/MainFrame.cpp',
                'src/Resources.cpp',
                'src/SlitherApp.cpp',
                'src/SlitherMath.cpp',
                'src/VideosGridDropTarget.cpp',
                'src/Worm.cpp',
                'src/WormTracker.cpp'],
    LIBS=env['LIBS'] + ['cxcore', 'cv', 'cvaux', 'highgui'])
env.Alias('slither', SlitherProgram)

# Build an Ubuntu package...
if 'ubuntu' in COMMAND_LINE_TARGETS:
    os.system("rm -Rf install/tmp")
    os.system("rm -f slither*.deb")
    SConscript("install/SConscript.Ubuntu", exports='env')

# Build an OS X disk image...
if 'osx' in COMMAND_LINE_TARGETS:
    os.system("rm -Rf install/tmp")
    os.system("rm -f slither*.dmg")
    SConscript("install/SConscript.OSX", exports='env')

