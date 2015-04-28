qmake -tp  vc -spec win32-msvc2010  pigale.pro
cd tgraph
qmake -tp vc -spec win32-msvc2010 tgraph.pro
cd ..\freeglut

qmake -tp vc -spec win32-msvc2010 freeglut.pro
cd ..\qt
qmake -tp vc -spec win32-msvc2010 qt.pro
cd ..\ClientServer
qmake -tp vc -spec win32-msvc2010 ClientServer.pro
cd ..\cgi
qmake -tp vc -spec win32-msvc2010 cgi.pro
cd ..\UsingTgraph
qmake -tp vc -spec win32-msvc2010 UsingTgraph.pro
cd ..
