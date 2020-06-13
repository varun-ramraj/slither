#
#   Slither.
#   Copyright (C) 2006-2020 Kip Warner. GPLv3 or later.
#
#   Process this file with automake to produce Makefile.in...
#

# Options for aclocal so it can find our custom M4 macros...
ACLOCAL_AMFLAGS = -I Macros

# Product list of programs destined for the binary prefix...
bin_PROGRAMS =                                                                  \
    slither

# These files must exist before anything is compiled. Can be machine generated...
BUILT_SOURCES =                                                                 \
    Source/Version.h

# Apply these pre-processor flags to all products...
AM_CPPFLAGS = -DSYSCONFDIR=\"$(sysconfdir)\"

# After studying Miller's Recursive Make Considered Harmful paper, I have
#  decided to consolidate all child makefiles into the parent. The only
#  directory to recurse into is i18n related and does not have any complex build
#  dependencies with external modules...
SUBDIRS = Translations

# Locale directory...
localedir = $(datadir)/locale

# Preprocessor definitions to pass down to the compiler on the actual command
#  line...
DEFS=
DEFS+= @DEFS@

# System manual pages...
man1_MANS =                                                                     \
    Documentation/slither.man

# Set slither build flags...
slither_CXXFLAGS            = $(CXXFLAGS)
slither_CPPFLAGS            = $(CPPFLAGS) $(AM_CPPFLAGS)
slither_LDADD               = $(LIBINTL) $(LIBS)
slither_LDFLAGS             = $(LDFLAGS)
slither_SOURCES             =                                                   \
    Source/AnalysisThread.cpp                                                   \
    Source/CaptureThread.cpp                                                    \
    Source/Experiment.cpp                                                       \
    Source/ImageAnalysisWindow.cpp                                              \
    Source/MainFrame.cpp                                                        \
    Source/Resources.cpp                                                        \
    Source/SlitherApp.cpp                                                       \
    Source/SlitherMath.cpp                                                      \
    Source/VideosGridDropTarget.cpp                                             \
    Source/Worm.cpp                                                             \
    Source/WormTracker.cpp

# Miscellaneous data files...
dist_pkgdata_DATA =                                                             \
    Resources/tips.txt                                                          \
    AUTHORS                                                                     \
    README.md                                                                   \
    TODO

# Always enable colour output for unit tests since sometimes automatic detection
#  of a capable terminal doesn't work according to Automake § 15.2.1...
AM_COLOR_TESTS=always

# Product list of scripts to generate during check target. Per § 15.2.1, Automake
#  ensures it will search for each listed file in both srcdir and . builddir...
check_SCRIPTS =                                                                 \
    TestRuntimeSane.sh

# GCS § 7.2.6 requires a check target. Targets to execute for check target...
TESTS = $(check_SCRIPTS) $(check_PROGRAMS)

# Manually add these things to distribution package when dist target is run...
EXTRA_DIST =                                                                    \
    Resources/analyze_32x32.xpm                                                 \
    Resources/book_64x64.xpm                                                    \
    Resources/camera.xpm                                                        \
    Resources/camera_64x64.xpm                                                  \
    Resources/experiment.icns                                                   \
    Resources/experiment.xcf                                                    \
    Resources/folder_64x64.xpm                                                  \
    Resources/pause_32x32.xpm                                                   \
    Resources/play_32x32.xpm                                                    \
    Resources/record_32x32.xpm                                                  \
    Resources/record_60x60.xpm                                                  \
    Resources/remove_32x32.xpm                                                  \
    Resources/rename_32x32.xpm                                                  \
    Resources/robot_64x64.xpm                                                   \
    Resources/save_60x60.xpm                                                    \
    Resources/skip_back_32x32.xpm                                               \
    Resources/skip_forward_32x32.xpm                                            \
    Resources/slither.icns                                                      \
    Resources/slither.png                                                       \
    Resources/slither.xpm                                                       \
    Resources/stop_32x32.xpm                                                    \
    Resources/stop_60x60.xpm                                                    \
    Resources/tips.txt                                                          \
    Resources/usb_32x32.xpm                                                     \
    AUTHORS                                                                     \
    README.md                                                                   \
    TODO                                                                        \
    Version.m4

# Make sure gettext is present...
check-gettext:
	@if test x$(USE_NLS) != "xyes" ; then echo "Missing gettext. Rerun configure and check for" \
	"'checking whether to use NLS... yes'!" ; exit 1 ; fi

# Force an update to the machine dependent message catalogs...
force-update-gmo: check-gettext
	find Translations/ -type f -name "*.po" -execdir touch {} \;
	cd Translations && $(MAKE) $(AM_MAKEFLAGS) update-gmo

# Before we prepare a distribution, run this hook...
dist-hook:
	$(RM) -r `find $(distdir) -name .git`
	$(RM) -r `find $(distdir) -name .hg`
	$(RM) -r `find $(distdir) -name .svn`

# Additional files to clean during normal clean target Automake couldn't
#  guess...
CLEANFILES =                                                                    \
	Source/Version.h                                                            \
	Translations/*.gmo                                                          \
    TestRuntimeSane.sh                                                          \
    $(check_PROGRAMS)                                                           \
    $(check_SCRIPTS)                                                            \
	$(dist_bin_SCRIPTS)

# Additional files left behind during dist target that need to be cleaned...
DISTCLEANFILES = \
	_configs.sed \
    $(top_builddir)/$(PACKAGE_TARNAME)-$(PACKAGE_VERSION).tar.*

# Actually clean all derived and maintainer files...
MAINTAINERCLEANFILES =                                                          \
	$(srcdir)/Auxiliary/config.h.in                                             \
	$(srcdir)/Auxiliary/config.guess                                            \
	$(srcdir)/Auxiliary/config.rpath                                            \
	$(srcdir)/Auxiliary/config.sub                                              \
	$(srcdir)/Auxiliary/depcomp                                                 \
	$(srcdir)/Auxiliary/install-sh                                              \
	$(srcdir)/Auxiliary/missing                                                 \
	$(srcdir)/Source/config.h.in                                                \
	$(srcdir)/Translations/Makevars.template                                    \
	$(srcdir)/ABOUT-NLS                                                         \
	$(srcdir)/aclocal.m4                                                        \
	$(srcdir)/config.guess                                                      \
	$(srcdir)/config.sub                                                        \
	$(srcdir)/configure                                                         \
	$(srcdir)/depcomp                                                           \
	$(srcdir)/install-sh                                                        \
	$(srcdir)/Makefile.in                                                       \
	$(srcdir)/missing

# Maintainer clean local hook to remove Autotools configuration output files...
maintainer-clean-local:
	$(RM) -r '$(srcdir)/Auxiliary'

# Rule to create test script to verify the server runtime works by just testing
#  to see it compiled and linked correctly...
TestRuntimeSane.sh: Makefile.am
	@echo 'set -e -u' > $@
	@echo '$(abs_builddir)/slither --version | $(GREP) -q "@PACKAGE_NAME@"' >> $@
	@$(CHMOD) +x $@

# Update the machine dependent message catalogs...
update-gmo: check-gettext
	cd Translations && $(MAKE) $(AM_MAKEFLAGS) update-gmo

# Create the .pot file...
#  TODO: This breaks if run via a VPATH / remote build.
#  xgettext(1) generates some warnings if < 0.19.8.1.334-c3290
#  <https://savannah.gnu.org/bugs/?50117>
update-po: check-gettext
	find $(srcdir)/ -iname "*.cpp" -print | sort > $(srcdir)/Translations/POTFILES.in.2 ; \
	find $(srcdir)/ -iname "*.cpp.in" -print | sort >> $(srcdir)/Translations/POTFILES.in.2 ; \
	find $(srcdir)/ \( -iname "*.h" ! -name config.h ! -name Fields.h ! -name Version.h \) -print | sort >> $(srcdir)/Translations/POTFILES.in.2 ; \
	find $(srcdir)/ -iname "*.h.in" -print | sort >> $(srcdir)/Translations/POTFILES.in.2 ; \
	if diff $(srcdir)/Translations/POTFILES.in $(srcdir)/Translations/POTFILES.in.2 >/dev/null 2>&1 ; then \
		$(RM) $(srcdir)/Translations/POTFILES.in.2 ; \
	else \
		mv $(srcdir)/Translations/POTFILES.in.2 $(srcdir)/Translations/POTFILES.in ; \
	fi
#	sed --in-place $(srcdir)/Translations/$(PACKAGE).pot --expression=s/CHARSET/UTF-8/ # CHARSET warning hack
	cd Translations && $(MAKE) $(AM_MAKEFLAGS) update-po

# Directive to make to let it know that these targets don't generate filesystem 
#  objects / products and therefore no need to check time stamps...
.PHONY: check-gettext force-update-gmo update-gmo update-po

