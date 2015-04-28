AUTOMAKE_OPTIONS = foreign
SYSTEM=mac
SUBDIRS =tgraph qt ClientServer UsingTgraph

EXTRA_DIST  = Doc/icones/*.png Doc/icones/*.gif Doc/icones/*.jpg  Doc/*.html Doc/*.css Doc/*.txt Doc/sourceforge.head 
EXTRA_DIST += tgf/*.tgf tgf/*.txt tgf/*.graphml 
EXTRA_DIST += incl/*.h incl/TAXI/*.h incl/QT/*.h incl/GL/*.h 
EXTRA_DIST += qt/icones/*.xpm qt/*.h qt/*.cpp qt/Action.awk qt/qt.pro qt/pigale_fr.ts qt/pigale_fr.qm  qt/qt_fr.qm qt/pigale.icns qt/pigale.ico qt/pigale.rc
EXTRA_DIST += ClientServer/*.h ClientServer/*.cpp ClientServer/ClientServer.pro ClientServer/ClientData.txt 
EXTRA_DIST += runqmake.sh Doxyfile macro/maxPlanar4Connex.mc
EXTRA_DIST += tgraph/*.cpp tgraph/tgraph.pro  tgraph/*.awk 
EXTRA_DIST += freeglut/*.c freeglut/*.h freeglut/freeglut.pro 
EXTRA_DIST += UsingTgraph/main.cpp UsingTgraph/UsingTgraph.pro UsingTgraph/Readme.txt
EXTRA_DIST += wpigale.inc pigale.inc pigale.pro  *.bat 
EXTRA_DIST += cgi/*.cpp cgi/*.h cgi/*.pro cgi/Readme.txt
datadir = $(prefix)
nobase_data_DATA = $(wildcard Doc/icones/*) $(wildcard Doc/*.*) $(wildcard tgf/*.*) macro/maxPlanar4Connex.mc
