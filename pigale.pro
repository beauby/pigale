TEMPLATE = subdirs


win32:SUBDIRS= tgraph \
         freeglut \
         qt \
         UsingTgraph \
         ClientServer

macx:SUBDIRS= tgraph \
         qt \
         UsingTgraph \
         ClientServer

CONFIG += ordered

