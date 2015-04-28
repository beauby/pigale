dnl Process this file with autoconf to produce a configure script.
AC_INIT(pigale,1.3.15,[hf@sourceforge.net])

dnl CURRENT:REVISION:AGE
LIB_VERSION=1.3.15
AC_SUBST(LIB_VERSION)
AC_PREFIX_DEFAULT(/usr/local/pigale-$PACKAGE_VERSION) 
if test "$prefix" != NONE; then
	PPATH="$prefix"
else
	PPATH="/usr/local/pigale-$PACKAGE_VERSION"
fi
AC_DEFINE_UNQUOTED(PACKAGE_PATH,"$PPATH",Path to Pigale root directory)
AC_SUBST(PPATH)
AC_SUBST(enable_static)

dnl Checks target processor
AC_CANONICAL_SYSTEM
dnl AC_CONFIG_SRCDIR([pigale-1.3.15])
AM_INIT_AUTOMAKE
AC_CONFIG_HEADER(incl/config.h:config.h.in)
AH_TOP([
#ifndef _CONFIG_H
#define _CONFIG_H
])
AH_BOTTOM([#endif])

#AC_ENABLE_STATIC
#AC_DISABLE_SHARED
#AC_PROG_LIBTOOL

dnl Checks for programs.
AC_PROG_AWK
AC_PROG_CXX
AC_LANG_CPLUSPLUS
AC_PROG_LN_S
AC_PROG_MAKE_SET

dnl OS
OS=`uname`
SYSTEM=xxxx

dnl Checks for headers
AC_HEADER_STDC
AC_CHECK_HEADERS([GL/gl.h GL/glu.h GL/glx.h unistd.h fcntl.h limits.h sys/ioctl.h sys/param.h sys/time.h])
AC_HEADER_TIME


AC_MSG_CHECKING([for QT])

AC_ARG_WITH(qt,
    [  --with-qt=DIR           where QT is installed. ],
    [  QT_PATH="$withval"],[QT_PATH=$QTDIR:/usr/share/qt5])


  echo qtpath: $QT_PATH   
if test "${OS}" = Darwin; then
    SYSTEM=mac
    AC_PATH_PROG(QT,[qmake],,[$QT_PATH/bin])
	if test "x$QT" = x; then
     AC_MSG_ERROR(qt: qmake not found)
	fi    
    QTDIR=${QT/\/bin\/qmake/}
    DIRS="tgraph qt ClientServer UsingTgraph"
else
	AC_PATH_PROG(QT,[bin/qmake],,[$QT_PATH/bin])
	if test "x$QT" = x; then
     AC_MSG_ERROR(qt: qmake not found !!)
	fi    
    QTDIR=${QT/\/bin\/qmake/}
    QT_INCLUDES="-I$QTDIR/include -I$QTDIR/include/Qt" 
    DIRS="tgraph freeglut qt ClientServer"
fi

AC_SUBST(DIRS)
AC_SUBST(QTDIR)

echo $QTDIR > qtdir


AC_ARG_ENABLE(debug,
[  --enable-debug          enable full pigale debugging [default=off]],
  enable_debug=$enableval, enable_debug=no)
if test "x$enable_debug" = xyes; then
	DEBUG=debug
else
	DEBUG=release
fi
AC_SUBST(DEBUG)


AC_ARG_ENABLE(ubuntu,
[  --enable-ubuntu         pigale will use the installed qt package [default=off]],
  enable_ubuntu=$enableval, enable_ubuntu=no)
if test "x$enable_ubuntu" = xyes; then
	SYSTEM=ubuntu
fi

AC_SUBST(SYSTEM)
echo $SYSTEM > system


AC_PATH_XTRA
LDFLAGS="${LDFLAGS} ${X_LIBS} ${X_EXTRA_LIBS}"


# Checks for library functions.
AC_PROG_GCC_TRADITIONAL
AC_FUNC_VPRINTF

echo "############### RESULTS: ######################"
echo host=$host
echo system=$SYSTEM
echo qmake=$QT
echo QTDIR=$QTDIR
echo install:$PPATH
echo cpu:$host_cpu

if test "x$AWK" = xgawk; then
	echo "found gawk"
else
	echo "Install gawk"
fi
echo static:$enable_static
AC_PROG_INSTALL
AC_CONFIG_FILES([ pigale.inc Makefile.am ])
AC_OUTPUT(
Makefile
)

./runqmake.sh

