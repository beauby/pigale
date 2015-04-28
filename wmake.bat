cd tgraph 
mingw32-make %1
cd ..\freeglut 
mingw32-make %1
cd ..\qt 
mingw32-make %1
cd ..\ClientServer
mingw32-make %1
cd ..


