# MQTDIR and MINGW are only used to copy dll
MQTDIR=C:\\Qt\\4.8.0m
MINGW=C:\\Qt\\qtcreator-2.4.0\\mingw
DISTPATH="C:\\Program Files\\pigale"
#to copy dll DLL=yes
DLL=yes
#if you are using QtCreator or Microsoft Visual Studio you better use MODE = debug_and_release
#MODE = release
#MODE = debug
MODE = debug_and_release

#QTDEBUGLIB = no
#debug_level 1-normal (used for profiler) 2-check arrays (slow)
DEBUG_LEVEL = 2
####################### Nothing to edit after this line ##################
DEFINES += MINGW _WINDOWS
SYSTEM=Windows
ENABLE_STATIC = yes