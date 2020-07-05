# Slither
# Copyright (C) 2006-2020 Kip Warner, GPLv3 or later.

# Minimum version of Autoconf used to successfully process configure.ac...
AC_PREREQ([2.67])

# Get package version...
m4_include([Version.m4])
m4_define([PACKAGE_VERSION_AC_INIT], [PACKAGE_VERSION_MAJOR.PACKAGE_VERSION_MINOR.PACKAGE_VERSION_PATCH[-git]PACKAGE_VERSION_GIT])

# Bake revision stamp into configure script. This allows us to examinate the
#  configure script without source control management to determine which
#  revision of configure.ac the configure script corresponds with...
AC_REVISION([PACKAGE_VERSION_AC_INIT])

# Initialize Autoconf...
AC_INIT(
    [Slither],
    PACKAGE_VERSION_AC_INIT,
    [Kip Warner <kip@thevertigo.com>],
    [slither],
    [https://github.com/kiplingw/slither])

# Clear default C / C++ compiler flags. Otherwise -g -O2 is automatically set
#  for the GNU C++ by AC_PROG_CC and AC_PROG_CXXCPP. Can still be overridden by
#  user. This needs to be right here after AC_INIT or these variables will get
#  clobbered...
: ${CFLAGS=""}
: ${CXXFLAGS=""}

# Location to find auxiliary automake scripts...
AC_CONFIG_AUX_DIR([Auxiliary])

# Location to find Autoconf plugins...
AC_CONFIG_MACRO_DIR([Macros])

# Preserve all arguments for products that want them into config.h...
config_flags="$0 $*"
AC_DEFINE_UNQUOTED(
    [CONFIGURATION_FLAGS],
    ["$config_flags"],
    [All flags that were passed to configure at configuration time.])

# Store build date stamp...
package_build_date=$(date --rfc-2822)
AC_DEFINE_UNQUOTED(
    [PACKAGE_BUILD_DATE],
    ["$package_build_date"],
    [Date and time this package was built.])
AC_SUBST([PACKAGE_BUILD_DATE], [$package_build_date])

# Copyright notice...
copyright_notice="Copyright (C) 2006-2020 Kip Warner. GPLv3 or later."
AC_COPYRIGHT([$copyright_notice])
AC_DEFINE_UNQUOTED(
    [COPYRIGHT_NOTICE],
    ["$copyright_notice"],
    [Author's copyright notice.])
AC_SUBST([COPYRIGHT_NOTICE], [$copyright_notice])

# Initialize Automake...

    # Setup flags. Add std-options later...
    AM_INIT_AUTOMAKE([
        color-tests     \
        dist-bzip2      \
        foreign         \
        no-dist-gzip    \
        silent-rules    \
        subdir-objects  \
        tar-pax         \
        -Wall           \
	-Werror])

    # Set by default to use silent build rules...
    AM_SILENT_RULES([yes])

    # Always enable maintainer mode, but allow user the option to disable it...
    AM_MAINTAINER_MODE([enable])

# Prepare canonical values for the platform we are compiling on and the platform
#  to host the generated files on...
AC_CANONICAL_BUILD
AC_CANONICAL_HOST

# Make host value available to source code...
AC_DEFINE_UNQUOTED(
    [HOST_CPU],
    ["$host_cpu"],
    [Host machine's CPU.])

# Sanity check to ensure generated ./configure script knows the directory on
#  which it is being executed is actually the project directory by providing any
#  unique source file...
AC_CONFIG_SRCDIR([Source/SlitherApp.cpp])

# Configure config.h...

    # Location to generate headers from templates containing Autoconf
    #  substitution variables...
    AC_CONFIG_HEADERS([Source/config.h])

# Check for command-line options...

    # Check for invalid command-line options. Autoconf § 15.5 says option
    #  checking is set to warn by default, but there appears to be a bug because
    #  general.m4 appears to set it to "no". We implement our own "fatal"
    #  behaviour...
    enable_option_checking=fatal
    if test -n "$ac_unrecognized_opts"; then
    case $enable_option_checking in
        no) ;;
        fatal) AC_MSG_ERROR([unrecognized option: $ac_unrecognized_opts]) ;;
        *) AC_MSG_WARN([unrecognized option: $ac_unrecognized_opts]) ;;
    esac
    fi

# Initialize i18n from GNU gettext...

    # Version 0.19.7 forward supports GtkBuilder XML, .desktop files, GSettings
    #  schemas, AppData, and Custom XML formats with ITS rules...
    #  Note: No white space indentation because of
    #  <https://savannah.gnu.org/bugs/index.php?52424>
AM_GNU_GETTEXT([external], [need-ngettext])
AM_GNU_GETTEXT_VERSION([0.19.8])

    # These options get passed to xgettext. These are the full GLib-approved
    #  list of options for use in a Gtk+ project...
    AM_XGETTEXT_OPTION([--from-code=UTF-8 --keyword=_ --keyword=N_ --keyword=C_:1c,2 --keyword=NC_:1c,2 --keyword=g_dngettext:2,3 --add-comments])

# Make sure paths are sane...

    # Alert user...
    AC_MSG_CHECKING([whether paths are sane...])

    # Build directory must not contain spaces...
    if echo $PWD | grep -E ' ' >/dev/null ; then
        AC_MSG_RESULT([no])
        AC_MSG_ERROR([your build path must not contain spaces...])
    fi

    # Source directory must not contain spaces...
    if echo $srcdir | grep -E ' ' >/dev/null ; then
        AC_MSG_RESULT([no])
        AC_MSG_ERROR([your source path must not contain spaces...])
    fi

    # Paths are sane...
    AC_MSG_RESULT([yes])

# Select our native language of C++ to perform tests in...
AC_LANG([C++])

# Checks for programs needed to build all subprojects and perform unit tests...

    # C++17 compiler with strict conformance required...
    #  TODO: Bump to C++20 when macro becomes available.
    AC_PROG_CXX
    AX_CXX_COMPILE_STDCXX_17([noext], [mandatory])

    # Take note of the compiler vendor...
    AX_COMPILER_VENDOR
    AC_DEFINE_UNQUOTED(
        [COMPILER_VENDOR],
        ["$ax_cv_cxx_compiler_vendor"],
        [Compiler vendor used to build everything.])

    # C++ preprocessor...
    AC_PROG_CXXCPP

    # Grep required for test suite...
    AC_PROG_GREP

    # install(1) utility to copy files and set attributes...
    AC_PROG_INSTALL

    # chmod(1)...
    AC_PATH_PROG([CHMOD], [chmod])

    # mkdir(1) -p functionality required in several places...
    AC_PROG_MKDIR_P

    # mktemp tool required to perform test suite...
    AC_PATH_PROG([MKTEMP], [mktemp])

    # Sed tool...
    AC_PROG_SED

    # rmdir necessary in various places...
    AC_PATH_PROG([RMDIR], [rmdir])
    if test "x$RMDIR" = "x"; then
        AC_MSG_ERROR([rmdir required, but unavailable...])
    fi

    # Valgrind optional, but used in unit testing if enabled by default...
    AX_VALGRIND_CHECK
    if test "x$enable_valgrind" = "xyes" && test "$VALGRIND" = ""; then
        AC_MSG_ERROR([valgrind required to perform test suite, but not found; either install or reconfigure with --disable-valgrind])
    fi

# Define some other preprocessor symbols of things our source needs...

    # Documentation directory...
    package_docdir=${prefix}/share/doc/${PACKAGE_TARNAME}
    AC_DEFINE_UNQUOTED(
        [DOCDIR],
        ["$package_docdir"],
        [Location documentation for this package was installed to.])

# Substitute autoconfig macros in various non-make related files...

    # Update version string...
    AC_CONFIG_FILES([Source/Version.h])

# Checks for system headers and features...

    # Figure out how to set a software traps on this particular hardware...
    AC_MSG_CHECKING([how to trigger software traps on $host_cpu])
    BREAKPOINT_TRAP=""
    case "$host_cpu" in

        # ARM: The bkpt instruction requires no operands...
        arm*)   BREAKPOINT_TRAP="asm volatile(\"bkpt\");" ;;

        # MIPS: Trap if both operands are equal...
        mips*)  BREAKPOINT_TRAP="asm volatile(\"teq \$0, \$0\");" ;;

        # POWER: Trap on all conditions (0x1f = 31) in comparing latter two
        #  operands. See s 3.3.11 Fixed-Point Trap Instructions within Power
        #  ISA v3.0B...
        power*)  BREAKPOINT_TRAP="asm volatile(\"tw 0b11111, 0, 0\");" ;;

        # Intel: Trap initiated on third interrupt...
        *i?86)   ;&
        x86_64) BREAKPOINT_TRAP="asm volatile(\"int \$0x03\");" ;;

    esac
    AC_DEFINE_UNQUOTED([BREAKPOINT_TRAP], $BREAKPOINT_TRAP, [Trigger a software trap on this architecture...])
    AC_MSG_RESULT([$BREAKPOINT_TRAP])

    # Standard C++ headers...
    AC_CHECK_HEADERS(
        [algorithm array cassert cctype cerrno chrono cstddef filesystem \
         iostream memory stdexcept string string_view vector],
        [], [AC_MSG_ERROR([missing some required standard C++ headers...])])

    # POSIX headers...
    AC_CHECK_HEADERS(
        [fcntl.h                                                                \
         iconv.h                                                                \
         sys/resource.h                                                         \
         sys/stat.h                                                             \
         sys/time.h                                                             \
         sys/types.h                                                            \
         unistd.h],
        [], [AC_MSG_ERROR([missing some required standard POSIX headers...])])

    # Check for system provided π constant...
    AC_MSG_CHECKING([whether system's cmath defines M_PI])
    AC_COMPILE_IFELSE(
        [AC_LANG_PROGRAM([#include <cmath>], [ double f = M_PI; (void)(f);])],
        [AC_MSG_RESULT([yes])],
        [
            # Alert user we're falling back...
            AC_MSG_RESULT([no, using our own])

            # This trick should work in any programming language...
            m_pi_workaround="(4 * ::atan(1.0))"
            AC_DEFINE_UNQUOTED(
                [M_PI],
                [$m_pi_workaround],
                [Workaround if no system definition for M_PI...])
            AC_SUBST([M_PI], [$m_pi_workaround])
        ])

    # Locates pkg-config tool on system and checks compatibility and sane...
    PKG_PROG_PKG_CONFIG([0.25])

# Check for package headers and libraries...

    # wxWidgets...
    AM_OPTIONS_WXCONFIG
    reqwx=3.0.0
    AM_PATH_WXCONFIG($reqwx, wxWin=1)
    if test "$wxWin" != 1; then
    AC_MSG_ERROR([
	    wxWidgets must be installed on your system.

	    Please check that wx-config is in path, the directory
	    where wxWidgets libraries are installed (returned by
	    'wx-config --libs' or 'wx-config --static --libs' command)
	    is in LD_LIBRARY_PATH or equivalent variable and
	    wxWidgets version is $reqwx or above.
	    ])
    fi
    CPPFLAGS="$CPPFLAGS $WX_CPPFLAGS"
    CXXFLAGS="$CXXFLAGS $WX_CXXFLAGS_ONLY"
    CFLAGS="$CFLAGS $WX_CFLAGS_ONLY"
    LIBS="$LIBS $WX_LIBS"

    # OpenCV...
    PKG_CHECK_MODULES(
        [opencv], [opencv4 >= 4], [],
        [AC_MSG_ERROR([opencv >= 4 missing...])])
    ALL_CXXFLAGS="$ALL_CXXFLAGS $opencv_CFLAGS"
    ALL_LIBS="$ALL_LIBS $opencv_LIBS"

# Checks for typedefs, structures, and compiler characteristics...

    # Endianness...
    AC_C_BIGENDIAN

    # Fixed width primitives...
    AC_TYPE_INT8_T
    AC_TYPE_INT16_T
    AC_TYPE_INT32_T
    AC_TYPE_UINT8_T
    AC_TYPE_UINT16_T
    AC_TYPE_UINT32_T

    # Other basic types...
    AC_TYPE_SIZE_T

# Checks for individual functions within libraries...

    # Required POSIX, Standard C, GNU C library extensions...
    AC_CHECK_FUNCS([                                                            \
        chdir                                                                   \
        close                                                                   \
        getenv                                                                  \
        getopt_long                                                             \
        realpath                                                                \
        strerror                                                                \
        sigaction                                                               \
        sigaddset                                                               \
        sigemptyset                                                             \
        sigwait                                                                 \
        strerror                                                                \
        umask                                                                   \
        write],
        [],
        [AC_MSG_ERROR([missing some POSIX, standard C, or GNU C library functions...])])

# Set additional compilation and linker flags...

    # Enable all warnings and treat them as errors...
    # -finline-limit=600
    CXXFLAGS="$CXXFLAGS -Wall -pipe -Werror -Wextra -Wshadow -fstack-protector-all --param=ssp-buffer-size=4 -Wformat -Wformat-security -Wsuggest-override"

# Generate makefiles from templates containing Autoconf substitution variables
#  for root makefile...
AC_CONFIG_FILES([               \
    Makefile                    \
    Translations/Makefile.in])

# Generates config.status which generates all files from templates...
AC_OUTPUT

# Show a user summary of the configuration...
echo \
"-----------------------------------------------------

${PACKAGE_NAME} ${PACKAGE_VERSION}
$copyright_notice

datadir.......................: $datadir
datarootdir...................: $datarootdir
libdir........................: $libdir
prefix........................: $prefix
sysconfdir....................: $sysconfdir

Compiler vendor...............: $ax_cv_cxx_compiler_vendor
Compiler......................: $CXX
Linker........................: $LD

C++ Flags.....................: $CXXFLAGS
C++ Preprocessor..............: $CPPFLAGS
Linker Flags..................: $LDFLAGS
Linker Libs...................: $LIBS

Now type 'make @<:@<target>@:>@' where the optional
<target> is:
    all            ...builds all products (default)
    check          ...perform all self diagnostics
    clean          ...clean the build
    dist           ...builds redistributable archive
    distcheck      ...self diagnostics on previous
    install        ...install to $prefix

-----------------------------------------------------"

