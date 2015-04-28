echo "Creating Makefiles"

if test -f qtdir ; then
  QTDIR=`cat qtdir`
  export QTDIR
fi
if test -f system ; then
  SYSTEM=`cat system`
fi
if [ $SYSTEM  = "ubuntu" ]; then
    QMAKE=/usr/bin/qmake-qt4
elif [ $SYSTEM  = "mac" ]; then
#    QMAKE="/usr/bin/qmake -spec macx-g++"
    QMAKE="$QTDIR/bin/qmake  -spec macx-g++"
else
    QMAKE=$QTDIR/bin/qmake 
fi

echo ${QMAKE} 
echo "*********************************************"
#${QMAKE}  -o Makefile  pigale.pro
cd tgraph
${QMAKE}  -o Makefile  tgraph.pro
echo 'end tgraph'
cd ../freeglut
${QMAKE}  -o Makefile  freeglut.pro
echo 'end free glut'
cd ../qt
${QMAKE}  -o Makefile  qt.pro
echo 'end qt'
cd ../ClientServer
${QMAKE}  -o Makefile  ClientServer.pro
echo 'end ClientServer'
cd ../cgi
${QMAKE}  -o Makefile  cgi.pro
echo 'end cgi'
cd ../UsingTgraph
${QMAKE}  -o Makefile  UsingTgraph.pro
cd ..
echo 'end UsingTgraph'

echo "*********************************************"
if [ $SYSTEM  = "mac" ]; then
    echo "Creating Xcode projects"
    ${QMAKE}  -spec macx-xcode pigale.pro 
    cd tgraph
    ${QMAKE}  -spec macx-xcode tgraph.pro
    cd ../qt
    ${QMAKE}  -spec macx-xcode qt.pro
    cd ../ClientServer
    ${QMAKE}  -spec macx-xcode ClientServer.pro
    cd ../UsingTgraph
    ${QMAKE}  -spec macx-xcode UsingTgraph.pro
    cd ../cgi
    ${QMAKE}  -spec macx-xcode cgi.pro
    cd ..
    ${QMAKE}  -spec macx-xcode pigale.pro
    echo "All Makefiles and Xcode projects created"
else
    echo "All project is configured"
fi


