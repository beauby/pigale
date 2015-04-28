TEMPLATE = app

INCLUDEPATH = ../incl
SOURCES = main.cpp
win32 {
      include(../wpigale.inc)
      win32-msvc2010:DEFINES += _CRT_SECURE_NO_WARNINGS
      }else{
      include(../pigale.inc)
}

CONFIG += $$MODE warn_on
CONFIG -= qt 
CONFIG += console
macx:CONFIG += x86 x86_64   
macx:CONFIG-=app_bundle   



CONFIG(debug, debug|release)  {
       LIBTGRAPH=libtgraph_debug.a
       win32-msvc2010:LIBTGRAPH=tgraph_debug.lib 
       TARGET=usingtgraph_debug
       OBJECTS_DIR = ./debug
       contains(DEBUG_LEVEL , 2) {
          DEFINES += TDEBUG
         }    
    }else { 
       LIBTGRAPH=libtgraph.a
       win32-msvc2010:LIBTGRAPH=tgraph.lib 
       TARGET=usingtgraph    
       OBJECTS_DIR = ./release
}

LIBS += ../lib/$$LIBTGRAPH
DESTDIR=$$PWD

# needed by ../makefile     
distdir.commands =
QMAKE_EXTRA_TARGETS += distdir

build_pass:CONFIG(debug, debug|release) {
    message(Debug: configuring $$TARGET)
    } else:build_pass{
    message(Release: configuring $$TARGET)
    }

#message(configuring UsingTgraph)


