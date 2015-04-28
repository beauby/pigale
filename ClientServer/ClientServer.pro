TEMPLATE = app

win32 {
      include(../wpigale.inc)
      } else {
      include(../pigale.inc)
      }

MOC_DIR = moc
INCLUDEPATH = ../incl
DEPENDPATH = ../incl
HEADERS = client.h 
SOURCES = client.cpp

CONFIG += qt thread $$MODE warn_on
QT += network widgets
macx:CONFIG += x86_64
#macx:CONFIG-=app_bundle
   
CONFIG(debug, debug|release)  {
    TARGET = client_debug
    DEFINES += TDEBUG
    OBJECTS_DIR = ./debug
    }else {
    TARGET = client
    unix:OBJECTS_DIR = ./release
    }


distdir.commands =
QMAKE_EXTRA_TARGETS += distdir
DESTDIR=$$PWD
   
#Installation
target.path = $$DISTPATH/bin	
data.files =ClientData.txt
data.path = $$DISTPATH/bin
INSTALLS += target data

build_pass:CONFIG(debug, debug|release) {
    message(Debug: configuring $$TARGET)
    } else:build_pass{
    message(Release: configuring $$TARGET)
}


message(configuring $$TARGET using QT version $$[QT_VERSION] ($$OBJECTS_DIR))
