TEMPLATE = app

win32 {
      include(../wpigale.inc)              
      QMAKE_CXXFLAGS_WARN_ON =  -Wall 
      QMAKE_CXXFLAGS += frtti -fexceptions 
      DEFINES +=  __MINGW32__ _WIN32 -DUNICODE -DQT_LARGEFILE_SUPPORT
      CONFIG += console
      LIBS +=  $$MQTDIR/lib/libQtNetwork4.a $$MQTDIR/lib/libQtCore4.a
      } else {
      include(../pigale.inc)
      }
      
OBJECTS_DIR = release  
MOC_DIR = moc
INCLUDEPATH += ../incl
DEPENDPATH += ../incl
DEFINES += QT_NO_DEBUG QT_NETWORK_LIB QT_CORE_LIB

SOURCES =  client.cpp\
	pigale.cpp

HEADERS = client.h 


CONFIG = qt thread release warn_on
macx {
     CONFIG += x86_64
     CONFIG-=app_bundle
}    

QT += opengl network  svg


TARGET=pigale.cgi
DESTDIR=$$PWD

//unix:DESTDIR=/var/www/cgi-bin
    build_pass:CONFIG(debug, debug|release) {
    message(Debug: configuring $$TARGET version:$$VERSION)
    } else:build_pass{
    message(Release: configuring $$TARGET version:$$VERSION)
    }

#message(configuring pigale.cgi using QT version $$[QT_VERSION] ($$OBJECTS_DIR))
