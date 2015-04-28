!macx {
TEMPLATE = lib
cache()

win32 {
      include(..\\wpigale.inc)
      DESTDIR="..\\lib"
      DEFINES = FREEGLUT_STATIC WIN32 _LIB _CRT_SECURE_NO_WARNINGS
      }else{
      include(../pigale.inc)
      DESTDIR = ../lib
      DEFINES = FREEGLUT_STATIC _LIB 
}

CONFIG(debug, debug|release) {
    TARGET = freeglut_debug 
    OBJECTS_DIR = ./debug
    DEFINES += _DEBUG
    } else {
    TARGET = freeglut
    OBJECTS_DIR = ./release
    DEFINES += NDEBUG
}    

CONFIG += thread create_prl static $$MODE
INCLUDEPATH = ../incl
unix::INCLUDEPATH += /usr/X11R6/include
DEPENDPATH = ../incl
QT += opengl
QT -= qt

SOURCES =\
freeglut_callbacks.c\
freeglut_cursor.c\
freeglut_display.c\
freeglut_ext.c\
freeglut_font.c\
freeglut_font_data.c\
freeglut_gamemode.c\
freeglut_geometry.c\
freeglut_glutfont_definitions.c\
freeglut_init.c\
freeglut_input_devices.c\
freeglut_joystick.c\
freeglut_main.c\
freeglut_menu.c\
freeglut_misc.c\
freeglut_overlay.c\
freeglut_spaceball.c\
freeglut_state.c\
freeglut_stroke_mono_roman.c\
freeglut_stroke_roman.c\
freeglut_structure.c\
freeglut_teapot.c\
freeglut_videoresize.c\
freeglut_window.c\
freeglut_xinput.c
      

unix {
     distdir.commands =
     QMAKE_EXTRA_TARGETS += distdir
     DESTDIR=$$DISTPATH/lib
}

message(configuring the library $$TARGET)
} else {
TEMPLATE = subdirs
message(FREEGLUT:Nothing to be done under MAC OSX)
}
