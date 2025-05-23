dnl Copyright (C) 1999-2022  Erik de Castro Lopo <erikd@mega-nerd.com>.

dnl Require autoconf version >= 2.69
AC_PREREQ([2.69])

AC_INIT([libsndfile],[1.2.2],[sndfile@mega-nerd.com],
		[libsndfile],[http://libsndfile.github.io/libsndfile/])

dnl Check whether we want to set defaults for CFLAGS, CXXFLAGS, CPPFLAGS and LDFLAGS
AC_MSG_CHECKING([whether configure should try to set CFLAGS/CXXFLAGS/CPPFLAGS/LDFLAGS])
AS_IF([test "x${CFLAGS+set}" = "xset" || test "x${CXXFLAGS+set}" = "xset" || test "x${CPPFLAGS+set}" = "xset" || test "x${LDFLAGS+set}" = "xset"], [
		enable_flags_setting=no
		: ${CFLAGS=""}
		: ${CXXFLAGS=""}
	], [
		enable_flags_setting=yes
		dnl Set to empty flags so AC_PROG_CC and
		dnl AC_PROG_CXX do not add -g -O2
		CFLAGS=""
		CXXFLAGS=""
	])
AC_MSG_RESULT([${enable_flags_setting}])

dnl Put config stuff in 'build-aux'.
AC_CONFIG_AUX_DIR([build-aux])

AC_CONFIG_SRCDIR([src/sndfile.c])
AC_CANONICAL_HOST

AC_CONFIG_MACRO_DIR([m4])
AC_CONFIG_HEADERS([src/config.h])

AM_INIT_AUTOMAKE([1.14 foreign dist-xz no-dist-gzip serial-tests subdir-objects])
AM_SILENT_RULES([yes])

dnl ====================================================================================

AC_PROG_CC
AC_PROG_CC_C99

AS_IF([test "x$ac_cv_prog_cc_c99" = "xno"], [
		AC_MSG_ERROR([libsndfile requires a C99 capable compiler!])
	])

AC_USE_SYSTEM_EXTENSIONS
AC_PROG_CXX

AC_LANG([C])
AX_COMPILER_VENDOR
AX_COMPILER_VERSION

AC_LANG_PUSH([C++])
AX_COMPILER_VENDOR
AX_COMPILER_VERSION
AC_LANG_POP([C++])

AC_PROG_SED
AM_PROG_AR

LT_INIT([disable-static win32-dll])
LT_PROG_RC

AC_PROG_INSTALL
AC_PROG_LN_S

AM_PATH_PYTHON

AC_CHECK_PROG([HAVE_AUTOGEN], [autogen], [yes], [no])
AC_CHECK_PROG([HAVE_WINE], [wine], [yes], [no])
AC_CHECK_PROG([HAVE_XCODE_SELECT], [xcode-select], [yes], [no])

dnl ------------------------------------------------------------------------------------
dnl  Rules for library version information:
dnl
dnl   1. Start with version information of `0:0:0' for each libtool library.
dnl   2. Update the version information only immediately before a public release of
dnl      your software. More frequent updates are unnecessary, and only guarantee
dnl      that the current interface number gets larger faster.
dnl   3. If the library source code has changed at all since the last update, then
dnl      increment revision (`c:r:a' becomes `c:r+1:a').
dnl   4. If any interfaces have been added, removed, or changed since the last update,
dnl      increment current, and set revision to 0.
dnl   5. If any interfaces have been added since the last public release, then increment
dnl      age.
dnl   6. If any interfaces have been removed since the last public release, then set age
dnl      to 0.

dnl   This is libtool version of library, we add it to `--version-info` property.

m4_define([lt_current], [1])
m4_define([lt_revision], [37])
m4_define([lt_age], [0])

dnl This is ABI version for linker scripts, CMake uses the same format for
dnl VERSION property of shared library.
dnl The formula is: c:r:a -> c-a:a:r

m4_define([abi_version_major], [m4_eval(lt_current - lt_age)])
m4_define([abi_version_minor], [lt_age])
m4_define([abi_version_patch], [lt_revision])

dnl ------------------------------------------------------------------------------------

AC_CHECK_HEADERS([endian.h])
AC_CHECK_HEADERS([byteswap.h])
AC_CHECK_HEADERS([locale.h])
AC_CHECK_HEADERS([sys/time.h])
AC_CHECK_HEADERS([immintrin.h])
AC_CHECK_HEADERS([stdbool.h])

AC_HEADER_SYS_WAIT

AC_CHECK_DECLS([S_IRGRP])
AS_IF([test "x$ac_cv_have_decl_S_IRGRP" = "xyes"], [
		AC_DEFINE_UNQUOTED([HAVE_DECL_S_IRGRP], [1], [Set to 1 if S_IRGRP is defined.])
	], [
		AC_DEFINE_UNQUOTED([HAVE_DECL_S_IRGRP], [0], [Set to 0 if S_IRGRP is not defined.])
	])

AM_CONDITIONAL([LINUX_MINGW_CROSS_TEST],
	[test "x${build_os}:${host_os}:${HAVE_WINE}" = "xlinux-gnu:mingw32msvc:yes"])

dnl ====================================================================================
dnl  Couple of initializations here. Fill in real values later.

SHLIB_VERSION_ARG=""

dnl ====================================================================================
dnl  Finished checking, handle options.

AC_ARG_ENABLE(experimental,
	AS_HELP_STRING([--enable-experimental], [enable experimental code]))

AS_IF([test "x$enable_experimental" = "xyes"], [
		EXPERIMENTAL_CODE=1
	], [
		EXPERIMENTAL_CODE=0
	])
AC_DEFINE_UNQUOTED([ENABLE_EXPERIMENTAL_CODE], [${EXPERIMENTAL_CODE}], [Set to 1 to enable experimental code.])

AC_ARG_ENABLE([werror],
	[AS_HELP_STRING([--enable-werror], [enable -Werror in all Makefiles])])

AC_ARG_ENABLE([bow-docs],
	[AS_HELP_STRING([--enable-bow-docs], [enable black-on-white html docs])])

AC_ARG_ENABLE([sqlite],
	[AS_HELP_STRING([--disable-sqlite], [disable use of sqlite])])

AC_ARG_ENABLE([alsa],
	[AS_HELP_STRING([--disable-alsa], [disable ALSA support (default=autodetect)])], [], [enable_alsa=auto])

AC_ARG_ENABLE([sndio],
	[AS_HELP_STRING([--disable-sndio], [disable Sndio support (default=autodetect)])], [], [enable_sndio=auto])

AC_ARG_ENABLE([external-libs],
	[AS_HELP_STRING([--disable-external-libs], [disable use of FLAC, Ogg and Vorbis [[default=no]]])])

AC_ARG_ENABLE([mpeg],
	[AS_HELP_STRING([--disable-mpeg], [disable use of LAME/MPG123 for MPEG (MP3) [[defaults=no]]])])

AC_ARG_ENABLE(octave,
	[AS_HELP_STRING([--enable-octave], [enable building of GNU Octave module])])

AC_ARG_ENABLE([full-suite],
	[AS_HELP_STRING([--disable-full-suite], [disable building and installing programs, documentation, only build library [[default=no]]])])
AM_CONDITIONAL([FULL_SUITE], [test "x$enable_full_suite" != "xno"])

AC_ARG_ENABLE([test-coverage],
	[AS_HELP_STRING([--enable-test-coverage], [enable test coverage])])
AM_CONDITIONAL([ENABLE_TEST_COVERAGE], [test "x$enable_test_coverage" = "xyes"])

AC_ARG_ENABLE([ossfuzzers],
	[AS_HELP_STRING([--enable-ossfuzzers], [Whether to generate the fuzzers for OSS-Fuzz])])
AM_CONDITIONAL([USE_OSSFUZZERS], [test "x$enable_ossfuzzers" = "xyes"])

AC_SUBST([LIB_FUZZING_ENGINE])
AM_CONDITIONAL([USE_OSSFUZZ_FLAG], [test "x$LIB_FUZZING_ENGINE" = "x-fsanitize=fuzzer"])
AM_CONDITIONAL([USE_OSSFUZZ_STATIC], [test -f "$LIB_FUZZING_ENGINE"])

dnl ====================================================================================
dnl  Check types and their sizes.

AC_CHECK_SIZEOF([wchar_t], [4])
AC_CHECK_SIZEOF([short], [2])
AC_CHECK_SIZEOF([int], [4])
AC_CHECK_SIZEOF([long], [4])
AC_CHECK_SIZEOF([float], [4])
AC_CHECK_SIZEOF([double], [4])
AC_CHECK_SIZEOF([void*], [8])
AC_CHECK_SIZEOF([size_t], [4])
AC_CHECK_SIZEOF([int64_t], [8])
AC_CHECK_SIZEOF([long long], [8])

dnl  Check for common 64 bit file offset types.
AC_CHECK_SIZEOF([off_t], [1])

AS_IF([test "x$enable_largefile:$ac_cv_sizeof_off_t" = "xno:8"], [
		AC_MSG_ERROR(["Error : Cannot disable large file support because sizeof (off_t) == 8."])
	])

AS_CASE([$host_os],
	[mingw32*], [
		AC_DEFINE([__USE_MINGW_ANSI_STDIO], [1], [Set to 1 to use C99 printf/snprintf in MinGW.])
		],

	[
		AS_IF([test "x$ac_cv_sizeof_off_t" = "x8"], [
				dnl If sizeof (off_t) is 8, no further checking is needed.
			], [
				dnl Save the old sizeof (off_t) value  and then unset it to see if it
				dnl changes when Large File Support is enabled.
				pre_largefile_sizeof_off_t=$ac_cv_sizeof_off_t
				unset ac_cv_sizeof_off_t

				AC_SYS_LARGEFILE
				AS_IF([test "x$ac_cv_sys_largefile_CFLAGS" = "xno"], [
						ac_cv_sys_largefile_CFLAGS=""
					])
				AS_IF([test "x$ac_cv_sys_largefile_LDFLAGS" = "xno"], [
						ac_cv_sys_largefile_LDFLAGS=""
					])
				AS_IF([test "x$ac_cv_sys_largefile_LIBS" = "xno"], [
						ac_cv_sys_largefile_LIBS=""
					])

				AC_CHECK_SIZEOF(off_t,1)
			])
	])

AC_TYPE_SSIZE_T

dnl ====================================================================================
dnl  Determine endian-ness of host processor.

AC_C_BIGENDIAN([
		dnl big-endian
		ac_cv_c_big_endian=1
		ac_cv_c_little_endian=0
	], [
		dnl little-endian
		ac_cv_c_big_endian=0
		ac_cv_c_little_endian=1
	])

AC_DEFINE_UNQUOTED([CPU_IS_BIG_ENDIAN], [${ac_cv_c_big_endian}], [Host processor is big endian.])
AC_DEFINE_UNQUOTED([CPU_IS_LITTLE_ENDIAN], [${ac_cv_c_little_endian}], [Host processor is little endian.])

dnl ====================================================================================
dnl  Check for functions.

AC_CHECK_FUNCS([malloc calloc realloc free])
AC_CHECK_FUNCS([open read write lseek lseek64])
AC_CHECK_FUNCS([fstat fstat64 ftruncate fsync])
AC_CHECK_FUNCS([snprintf vsnprintf])
AC_CHECK_FUNCS([gmtime gmtime_r localtime localtime_r gettimeofday])
AC_CHECK_FUNCS([mmap getpagesize])
AC_CHECK_FUNCS([setlocale])
AC_CHECK_FUNCS([pipe waitpid])

AC_SEARCH_LIBS([floor], [m], [], [
		AC_MSG_ERROR([unable to find the floor() function!])
	])
AC_CHECK_FUNCS([floor ceil fmod lrint lrintf])

dnl ====================================================================================
dnl  Check for requirements for building plugins for other languages/environments.

dnl Octave maths environment http://www.octave.org/
AS_IF([test "x$cross_compiling" = "xno"], [
		AS_IF([test "x$enable_octave" = "xno"], [
				AM_CONDITIONAL(BUILD_OCTAVE_MOD, false)
			], [
				AC_OCTAVE_BUILD
			])
	], [
		AM_CONDITIONAL(BUILD_OCTAVE_MOD, false)
	])

dnl ====================================================================================
dnl  Check for Ogg, Vorbis and FLAC.

HAVE_EXTERNAL_XIPH_LIBS=0
EXTERNAL_XIPH_CFLAGS=""
EXTERNAL_XIPH_LIBS=""
EXTERNAL_XIPH_REQUIRE=""

dnl Check for pkg-config outside the if statement.
PKG_PROG_PKG_CONFIG
AX_REQUIRE_DEFINED([PKG_INSTALLDIR])
PKG_INSTALLDIR

AS_IF([test -n "$PKG_CONFIG"], [
		AS_IF([test "x$enable_external_libs" = "xno"], [
				AC_MSG_WARN([[*** External libs (FLAC, Ogg, Vorbis) disabled. ***]])
			], [
				PKG_CHECK_MOD_VERSION(FLAC, flac >= 1.3.1, ac_cv_flac=yes, ac_cv_flac=no)

				dnl Make sure the FLAC_CFLAGS value is sane.
				FLAC_CFLAGS=`echo $FLAC_CFLAGS | $SED "s|include/FLAC|include|"`

				PKG_CHECK_MOD_VERSION(OGG, ogg >= 1.3.0, ac_cv_ogg=yes, ac_cv_ogg=no)

				AS_IF([test "x$enable_experimental" = "xyes"], [
						PKG_CHECK_MOD_VERSION(SPEEX, speex >= 1.2, ac_cv_speex=yes, ac_cv_speex=no)
					], [
						SPEEX_CFLAGS=""
						SPEEX_LIBS=""
					])

				dnl Vorbis versions earlier than 1.2.3 have bugs that cause the libsndfile
				dnl test suite to fail on MIPS, PowerPC and others.
				dnl See: http://bugs.debian.org/cgi-bin/bugreport.cgi?bug=549899
				PKG_CHECK_MOD_VERSION(VORBIS, vorbis >= 1.2.3, ac_cv_vorbis=yes, ac_cv_vorbis=no)
				PKG_CHECK_MOD_VERSION(VORBISENC, vorbisenc >= 1.2.3, ac_cv_vorbisenc=yes, ac_cv_vorbisenc=no)

				PKG_CHECK_MOD_VERSION(OPUS, opus >= 1.1, ac_cv_opus=yes, ac_cv_opus=no)
				enable_external_libs=yes
			])

			AS_IF([test "x$ac_cv_flac$ac_cv_ogg$ac_cv_vorbis$ac_cv_vorbisenc$ac_cv_opus" = "xyesyesyesyesyes"], [
					HAVE_EXTERNAL_XIPH_LIBS=1
					enable_external_libs=yes

					EXTERNAL_XIPH_CFLAGS="$FLAC_CFLAGS $VORBIS_CFLAGS $VORBISENC_CFLAGS $SPEEX_CFLAGS $OPUS_CFLAGS $OGG_CFLAGS "
					EXTERNAL_XIPH_LIBS="$FLAC_LIBS $VORBIS_LIBS $VORBISENC_LIBS $SPEEX_LIBS $OPUS_LIBS $OGG_LIBS "
					EXTERNAL_XIPH_REQUIRE="flac ogg vorbis vorbisenc opus"

					if test x$ac_cv_speex = "xyes" ; then
						EXTERNAL_XIPH_REQUIRE="$EXTERNAL_XIPH_REQUIRE speex"
					fi
				], [
					AS_ECHO([""])
					AC_MSG_WARN([[*** One or more of the external libraries (ie libflac, libogg,]])
					AC_MSG_WARN([[*** libvorbis and libopus) is either missing (possibly only the development]])
					AC_MSG_WARN([[*** headers) or is of an unsupported version.]])
					AC_MSG_WARN([[***]])
					AC_MSG_WARN([[*** Unfortunately, for ease of maintenance, the external libs]])
					AC_MSG_WARN([[*** are an all or nothing affair.]])
					AS_ECHO([""])
					enable_external_libs=no
				])
	])

AC_DEFINE_UNQUOTED([HAVE_EXTERNAL_XIPH_LIBS], [$HAVE_EXTERNAL_XIPH_LIBS], [Will be set to 1 if flac, ogg, vorbis, and opus are available.])

dnl ====================================================================================
dnl  Check for MPEG libraries liblame

ac_cv_lame="no"
ac_cv_mpg123="no"
HAVE_MPEG=0
EXTERNAL_MPEG_REQUIRE=""
EXTERNAL_MPEG_LIBS=""

AS_IF([test -n "$PKG_CONFIG"], [
		AS_IF([test "x$enable_mpeg" = "xno"], [
				AC_MSG_WARN([[*** MPEG (Lame/MPG123) disabled. ***]])
			], [
				AC_CHECK_HEADER(lame/lame.h,
								lame_header_found="yes",
								lame_header_found="no")
				AC_SEARCH_LIBS(lame_set_VBR_q, [lame mp3lame], [lame_lib_found="yes"], [lame_lib_found="no"])
				AS_IF([test "x$lame_lib_found$lame_header_found" = "xyesyes"], [
						ac_cv_lame="yes"
					], [
						AC_MSG_WARN([["MPEG support selected but external Lame library cannot be found.]])
					])

				PKG_CHECK_MOD_VERSION([MPG123], [libmpg123 >= 1.25.10], [
						ac_cv_mpg123="yes"
					], [
						AC_MSG_WARN([["MPEG support selected but external MPG123 library cannot be found.]])
					])

				AS_IF([test "x$ac_cv_lame$ac_cv_mpg123" = "xyesyes"], [
						enable_mpeg="yes"
						HAVE_MPEG=1
						EXTERNAL_MPEG_REQUIRE="libmpg123"
						EXTERNAL_MPEG_LIBS="-lmp3lame"
						MPEG_CFLAGS="$MPG123_CFLAGS"
						MPEG_LIBS="$MPG123_LIBS"
					], [
						enable_mpeg="no"
						AS_ECHO([""])
						AC_MSG_WARN([[*** MPEG support disabled.]])
						AS_ECHO([""])
					])
			])
	])

AC_DEFINE_UNQUOTED([HAVE_MPEG], [$HAVE_MPEG], [Will be set to 1 if lame, mpg123 mpeg support is available.])

dnl ====================================================================================
dnl  Check for libsqlite3 (only used in regtest).

ac_cv_sqlite3=0
AS_IF([test "x$enable_sqlite" != "xno"], [
		PKG_CHECK_MOD_VERSION([SQLITE3], [sqlite3 >= 3.2], [ac_cv_sqlite3=1], [ac_cv_sqlite3=0])
	])

AC_DEFINE_UNQUOTED([HAVE_SQLITE3], [${ac_cv_sqlite3}], [Set to 1 if you have libsqlite3.])
AM_CONDITIONAL([HAVE_SQLITE3], [test "x$ac_cv_sqlite3" = "x1"])

dnl ====================================================================================
dnl  Host OS specific stuff.

OS_SPECIFIC_CFLAGS=""
OS_SPECIFIC_LINKS=""
os_is_win32=0
os_is_openbsd=0
use_windows_api=0
AS_CASE([$host_os],
	[darwin* | rhapsody*], [
		AS_IF([test "x$HAVE_XCODE_SELECT" = "xyes"], [
				developer_path=`xcode-select --print-path`
			], [
				developer_path="/Developer"
			])
		OS_SPECIFIC_LINKS="-framework CoreAudio -framework AudioToolbox -framework CoreFoundation"],
	[mingw*], [
		os_is_win32=1
		use_windows_api=1
		OS_SPECIFIC_LINKS="-lwinmm"],
	[openbsd*], [
		os_is_openbsd=1
	])

AC_DEFINE_UNQUOTED([OS_IS_WIN32], [${os_is_win32}], [Set to 1 if compiling for Win32])
AC_DEFINE_UNQUOTED([OS_IS_OPENBSD], [${os_is_openbsd}], [Set to 1 if compiling for OpenBSD])
AC_DEFINE_UNQUOTED([USE_WINDOWS_API], [${use_windows_api}], [Set to 1 to use the native windows API])
AM_CONDITIONAL(USE_WIN_VERSION_FILE, test ${use_windows_api} -eq 1)

dnl ====================================================================================
dnl  Check for ALSA.

AS_IF([test "x$enable_alsa" != "xno"], [
		PKG_CHECK_MODULES([ALSA], [alsa], [
				dnl actually test whether ALSA really works, in
				dnl order to dodge wrong cross-compilation pickups
				save_CFLAGS="${CFLAGS}"
				save_LIBS="${LIBS}"
				CFLAGS="${CFLAGS} ${ALSA_CFLAGS}"
				LIBS="${LIBS} ${ALSA_LIBS}"
				AC_CHECK_HEADERS([alsa/asoundlib.h])
				AS_IF([test "x$ac_cv_header_alsa_asoundlib_h" = "xyes"], [
						dnl ALSA definitely works
						AC_DEFINE([HAVE_ALSA], [1], [Set to 1 if you have alsa])
						alsa_works="yes"
					], [
						dnl picked up wrong ALSA
						alsa_works="no"

						dnl reset flags
						ALSA_CFLAGS=""
						ALSA_LIBS=""
					])
				CFLAGS="${save_CFLAGS}"
				LIBS="${save_LIBS}"
			], [
				dnl could not find ALSA
				alsa_works="no"
			])

		AS_IF([test "x$alsa_works" = "xno"], [
				AS_IF([test "x$enable_alsa" = "xyes"], [
						dnl explicitly passed --enable-alsa, hence error out loud and clearly
						AC_MSG_ERROR([You explicitly requested alsa support, but alsa could not be found!])
					], [
						dnl did not explicitly pass --enable-alsa, relying on default automagic on
						enable_alsa="no (auto)"
					])
			])
	])

dnl ====================================================================================
dnl  Check for Sndio.

HAVE_SNDIO_H=0
AS_IF([test "x$alsa_works" != "xyes"], [
		AS_IF([test "x$enable_sndio" != "xno"], [
				PKG_CHECK_MODULES([SNDIO], [sndio], [
						HAVE_SNDIO_H=1
						ac_cv_sndio="yes"
					], [
						ac_cv_sndio="no"
					])
		
				AS_IF([test "x$ac_cv_sndio" = "xno"], [
						AS_IF([test "x$enable_sndio" = "xyes"], [
								dnl explicitly passed --enable-sndio, hence error out loud and clearly
								AC_MSG_ERROR([You explicitly requested sndio support, but sndio could not be found!])
							], [
								dnl did not explicitly pass --enable-sndio, relying on default automagic on
								enable_sndio="no (auto)"
							])
					])
			])
	], [
		AC_MSG_WARN([You have requested ALSA already, disable ALSA if you want to use Sndio.])
	])

AC_DEFINE_UNQUOTED([HAVE_SNDIO_H], [${HAVE_SNDIO_H}], [Set to 1 if <sndio.h> is available.])

dnl ====================================================================================
dnl  Test for sanity when cross-compiling.

AS_IF([test "x$ac_cv_sizeof_short" != "x2"], [
		AC_MSG_WARN([[******************************************************************]])
		AC_MSG_WARN([[*** sizeof (short) != 2.                                          ]])
		AC_MSG_WARN([[******************************************************************]])
	])

AS_IF([test "x$ac_cv_sizeof_int" != "x4"], [
		AC_MSG_WARN([[******************************************************************]])
		AC_MSG_WARN([[*** sizeof (int) != 4                                             ]])
		AC_MSG_WARN([[******************************************************************]])
	])

AS_IF([test "x$ac_cv_sizeof_float" != "x4"], [
		AC_MSG_WARN([[******************************************************************]])
		AC_MSG_WARN([[*** sizeof (float) != 4.                                          ]])
		AC_MSG_WARN([[******************************************************************]])
	])

AS_IF([test "x$ac_cv_sizeof_double" != "x8"], [
		AC_MSG_WARN([[******************************************************************]])
		AC_MSG_WARN([[*** sizeof (double) != 8.                                         ]])
		AC_MSG_WARN([[******************************************************************]])
	])

AM_CONDITIONAL([HAVE_AUTOGEN], [test "x$ac_cv_prog_HAVE_AUTOGEN" = "xyes"])

dnl ====================================================================================
dnl  Settings for the HTML documentation.

AS_IF([test "x$enable_bow_docs" = "xyes"], [
		HTML_BGCOLOUR="white"
		HTML_FGCOLOUR="black"
	], [
		HTML_BGCOLOUR="black"
		HTML_FGCOLOUR="white"
	])

dnl ====================================================================================
dnl  Now use the information from the checking stage.

win32_target_dll=0
COMPILER_IS_GCC=0

AS_IF([test "x$enable_flags_setting" = "xyes"], [
		AX_APPEND_COMPILE_FLAGS([-O2 -pipe], [CFLAGS])

		AC_LANG_PUSH([C++])
		AX_APPEND_COMPILE_FLAGS([-O2 -pipe], [CXXFLAGS])
		AC_LANG_POP([C++])

		AS_CASE([${host_os}],
			[darwin*], [
				ldflags_test="-Wl,-dead_strip_dylibs"],
			[linux*], [
				ldflags_test="-Wl,-O1 -Wl,--as-needed -Wl,--no-undefined -Wl,--gc-sections"]
			)
		AX_APPEND_LINK_FLAGS([${ldflags_test}], [LDFLAGS])
	])

AS_IF([test "x$enable_werror" = "xyes"], [
		AX_APPEND_COMPILE_FLAGS([-Werror], [CFLAGS])

		AC_LANG_PUSH([C++])
		AX_APPEND_COMPILE_FLAGS([-Werror], [CXXFLAGS])
		AC_LANG_POP([C++])
	])

common_flags="-Wall -Wextra -Wpointer-arith -Wcast-align -Wcast-qual -Wshadow -Wwrite-strings -Wundef -Wuninitialized -Winit-self -Wno-format-truncation"
common_cflags="${common_flags}"
common_cxxflags="${common_flags}"

AX_APPEND_COMPILE_FLAGS([${common_cflags} -Wvla -Wbad-function-cast -Wnested-externs -Wstrict-prototypes -Wmissing-prototypes -Wmissing-declarations -Waggregate-return], [CFLAGS])

AC_LANG_PUSH([C++])
AX_APPEND_COMPILE_FLAGS([${common_cxxflags} -Wctor-dtor-privacy -Wnon-virtual-dtor -Woverloaded-virtual -Wreorder -Wsign-promo], [CXXFLAGS])
AC_LANG_POP([C++])

AS_IF([test "x$enable_test_coverage" = "xyes"], [
		AX_APPEND_COMPILE_FLAGS([-coverage], [CFLAGS])
	])

dnl some distributions (such as Gentoo) have _FORTIFY_SOURCE always
dnl enabled. We test for this situation in order to prevent polluting
dnl the console with messages of macro redefinitions.
AX_ADD_FORTIFY_SOURCE

AS_IF([test "x$ax_cv_c_compiler_vendor" = "xgnu"], [
		dnl OS specific tweaks.
		AS_CASE([$host_os],
			[darwin* | rhapsody*], [
				dnl Disable -Wall, -pedantic and -Wshadow for Apple Darwin/Rhapsody.
				dnl System headers on these systems are broken.
				temp_CFLAGS=`echo $CFLAGS | $SED "s/-Wall -pedantic//" | $SED "s/-Wshadow//" | $SED "s/-Waggregate-return//"`
				CFLAGS=$temp_CFLAGS
				SHLIB_VERSION_ARG="-Wl,-exported_symbols_list -Wl,\$(top_srcdir)/src/Symbols.darwin"],
			[mingw*], [
				SHLIB_VERSION_ARG="-Wc,-static-libgcc -Wl,\$(top_srcdir)/src/libsndfile-1.def"
				win32_target_dll=1
				AS_IF([test "x$enable_shared" = "xno"], [
						win32_target_dll=0
					])
				AX_APPEND_COMPILE_FLAGS([-mstackrealign], [CFLAGS])
				],
			[os2*], [
				SHLIB_VERSION_ARG="-Wl,-export-symbols \$(top_srcdir)/src/Symbols.os2"
			])

		COMPILER_IS_GCC=1
	])

AS_CASE([$host_os], [linux*|kfreebsd*-gnu*|gnu*],  [SHLIB_VERSION_ARG="-Wl,--version-script=\$(top_srcdir)/src/Symbols.gnu-binutils"])

AC_DEFINE_UNQUOTED([WIN32_TARGET_DLL], [${win32_target_dll}], [Set to 1 if windows DLL is being built.])
AC_DEFINE_UNQUOTED([COMPILER_IS_GCC], [${COMPILER_IS_GCC}], [Set to 1 if the compile is GNU GCC.])

CFLAGS="$CFLAGS $OS_SPECIFIC_CFLAGS"

AS_IF([test "x$CFLAGS" = "x"], [
		AC_MSG_ERROR(["Error in configure script. CFLAGS has been screwed up."])
	])

HOST_TRIPLET="${host_cpu}-${host_vendor}-${host_os}"

AC_DEFINE_UNQUOTED([HOST_TRIPLET], [${HOST_TRIPLET}], [The host triplet of the compiled binary.])

AS_IF([test "$HOST_TRIPLET" = "x86_64-w64-mingw32"], [
		OS_SPECIFIC_LINKS=" -static-libgcc $OS_SPECIFIC_LINKS"
	])

WIN_RC_VERSION=`echo $PACKAGE_VERSION | $SED -e "s/pre.*//" -e "s/beta.*//" -e "s/\./,/g"`


AS_IF([test "x$enable_static" = "xno"], [
		SRC_BINDIR=src/.libs/
		TEST_BINDIR=tests/.libs/
	], [
		SRC_BINDIR=src/
		TEST_BINDIR=tests/
	])

dnl -------------------------------------------------------------------------------

AC_SUBST(HOST_TRIPLET)

AC_SUBST(HTML_BGCOLOUR)
AC_SUBST(HTML_FGCOLOUR)

AC_SUBST(SHLIB_VERSION_ARG)
AC_SUBST([SHARED_VERSION_INFO], [lt_current:lt_revision:lt_age])
AC_SUBST([ABI_VERSION], [abi_version_major.abi_version_minor.abi_version_patch])
AC_SUBST(CLEAN_VERSION)
AC_SUBST(VERSION_MAJOR)
AC_SUBST(GEN_TOOL)

AC_SUBST(WIN_RC_VERSION)

AC_SUBST(HAVE_EXTERNAL_XIPH_LIBS)
AC_SUBST(HAVE_MPEG)
AC_SUBST(OS_SPECIFIC_CFLAGS)
AC_SUBST(OS_SPECIFIC_LINKS)
AC_SUBST(SNDIO_LIBS)

AC_SUBST(EXTERNAL_XIPH_CFLAGS)
AC_SUBST(EXTERNAL_XIPH_LIBS)
AC_SUBST(EXTERNAL_XIPH_REQUIRE)
AC_SUBST(EXTERNAL_MPEG_LIBS)
AC_SUBST(EXTERNAL_MPEG_REQUIRE)
AC_SUBST(MPG123_CFLAGS)
AC_SUBST(MPG123_LIBS)
AC_SUBST(MPEG_CFLAGS)
AC_SUBST(MPEG_LIBS)
AC_SUBST(SRC_BINDIR)
AC_SUBST(TEST_BINDIR)

AC_CONFIG_FILES([
	Makefile Octave/Makefile
	src/version-metadata.rc
	tests/test_wrapper.sh tests/pedantic-header-test.sh
	sndfile.pc
	Scripts/build-test-tarball.mk
	])
AC_OUTPUT

dnl ====================================================================================

AS_IF([test -z "$PKG_CONFIG"], [
		AS_ECHO(["    *****************************************************************"])
		AS_ECHO(["    ***          The pkg-config program is missing.               ***"])
		AS_ECHO(["    *** External FLAC/Ogg/Vorbis libs cannot be found without it. ***"])
		AS_ECHO(["    ***       http://pkg-config.freedesktop.org/wiki/             ***"])
		AS_ECHO(["    *****************************************************************"])
	])

AX_RECURSIVE_EVAL([$libdir], [full_absolute_libdir])
AX_RECURSIVE_EVAL([$bindir], [full_absolute_bindir])
AX_RECURSIVE_EVAL([$pkgconfigdir], [full_absolute_pkgconfigdir])
AX_RECURSIVE_EVAL([$htmldir], [full_absolute_htmldir])
AC_MSG_RESULT([
-=-=-=-=-=-=-=-=-=-= Configuration Complete =-=-=-=-=-=-=-=-=-=-=-

  Configuration summary :

    libsndfile version : .................. ${VERSION}

    Host CPU : ............................ ${host_cpu}
    Host Vendor : ......................... ${host_vendor}
    Host OS : ............................. ${host_os}

    CFLAGS : .............................. ${CFLAGS}
    CXXFLAGS : ............................ ${CXXFLAGS}
    CPPFLAGS : ............................ ${CPPFLAGS}
    LDFLAGS : ............................. ${LDFLAGS}

    Experimental code : ................... ${enable_experimental:-no}
    Using ALSA in example programs : ...... ${enable_alsa:-no}
    External FLAC/Ogg/Vorbis/Opus : ....... ${enable_external_libs:-no}
    External MPEG Lame/MPG123 : ........... ${enable_mpeg:-no}
    Building Octave interface : ........... ${OCTAVE_BUILD}

  Tools :

    C Compiler Vendor is : ................ ${ax_cv_c_compiler_vendor} (${ax_cv_c_compiler_version})
    CXX Compiler Vendor is : .............. ${ax_cv_cxx_compiler_vendor} (${ax_cv_cxx_compiler_version})

    Sanitizer enabled : ................... ${enable_sanitizer:-no}

  Installation directories :

    Library directory : ................... ${full_absolute_libdir}
    Program directory : ................... ${full_absolute_bindir}
    Pkgconfig directory : ................. ${full_absolute_pkgconfigdir}
    HTML docs directory : ................. ${full_absolute_htmldir}

Compiling some other packages against libsndfile may require
the addition of '$full_absolute_pkgconfigdir' to the
PKG_CONFIG_PATH environment variable.
])

dnl Remove symlink created by Scripts/android-configure.sh.
rm -f gdbclient
