SYSTEM=mac
contains(SYSTEM,"ubuntu") {
        QTINCLUDE=/usr/include/qt4
        GLUTLIB=/usr/lib/libglut.a
        QTLRELEASE=/usr/share/qt4/bin/lrelease
        QTLUPDATE=/usr/share/qt4/bin/lupdate
        } else {
          contains(SYSTEM,"mac") {
          QTLRELEASE= /Users/hubert/Qt5.2.0/5.2.0/clang_64//bin/lrelease
          QTLUPDATE= /Users/hubert/Qt5.2.0/5.2.0/clang_64//bin/lupdate		  
          } else {
          QTLRELEASE= /Users/hubert/Qt5.2.0/5.2.0/clang_64//bin/lrelease
          QTLUPDATE= /Users/hubert/Qt5.2.0/5.2.0/clang_64//bin/lupdate
          GLUTLIB=/Users/hubert/pigale-1.3.15/lib/libglut.a
          }
        MQTDIR = /Users/hubert/Qt5.2.0/5.2.0/clang_64/
        QTINCLUDE= /Users/hubert/Qt5.2.0/5.2.0/clang_64//include/Qt
        }
#debug_level 1-normal (used for profiler) 2-check arrays (slow)
DEBUG_LEVEL = 2
MODE = release
####################### Nothing to edit after this line ##################
#version of the library
VERSION= 1.3.15
DISTPATH = /Users/hubert/pigale-1.3.15
AWK = awk
#QMAKE_CXXFLAGS_RELEASE =  @CPU@


