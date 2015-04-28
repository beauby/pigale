TEMPLATE = lib

win32 {
      include(../wpigale.inc)
      win32-msvc2010:DEFINES+= _CRT_SECURE_NO_WARNINGS
      }else{
      include(../pigale.inc)
}

INCLUDEPATH = ../incl
DEPENDPATH = ../incl

SOURCES =\
    3-ConOrientTriang.cpp\
    3-ConShelling1.cpp\
    3-ConShelling2.cpp\
    Barycenter.cpp\
    BFS.cpp\
    Biconnect.cpp\
    Bipolar.cpp\
    BipPlanarize.cpp\
    CotreeCritical.cpp\
    Debug.cpp\
    DFS.cpp\
    DFSGraph.cpp\
    DFSMap.cpp\
    Diagonalise.cpp\
    Embed2Pages.cpp\
    EmbedPolrec.cpp\
    EmbedFPP.cpp\
    EmbedCCD.cpp\
    EmbedPolyline.cpp\
    EmbedTutteCircle.cpp\
    EmbedVision.cpp\
    FastTwit.cpp\
    File.cpp\
    Generate.cpp\
    GeoAlgs.cpp\
    Graph.cpp\
    HeapSort.cpp\
    k-InfOrient.cpp\
    LR-Algo.cpp\
    LR-Map.cpp\
    mark.cpp\
    MaxPlanar.cpp\
    more.cpp\
    netcut.cpp\
    NewPolar.cpp\
    NpBiconnect.cpp\
    Planar.cpp\
    PropName.cpp\
    PropTgf.cpp\
    PSet.cpp\
    random.cpp\
    reduce.cpp\
    SchaefferGen.cpp\
    Schnyder.cpp\
    SchnyderWood.cpp\
    SWShelling.cpp\
    STList.cpp\
    TestSinglePassPlanar.cpp\
    Tgf.cpp\
    TopoAlgs.cpp\
    Twit.cpp\
    VertexPacking.cpp\
    Vision.cpp


CONFIG += thread warn_on create_prl
CONFIG += static
CONFIG -=qt
//CONFIG += debug_and_release
macx:CONFIG += x86_64
macx:DEFINES += OSX

CONFIG(debug, debug|release)  {
    TARGET = tgraph_debug
    OBJECTS_DIR = ./debug
    contains(DEBUG_LEVEL , 2) {
    DEFINES += TDEBUG
    }    
    }else {
    TARGET = tgraph
    OBJECTS_DIR = ./release
    }
    
DESTDIR=../lib

unix {
     distdir.commands =
     QMAKE_EXTRA_TARGETS += distdir
      # awk
      awk.target = PropName.cpp
      awk.depends = propname.awk ../incl/TAXI/propdef.h
      awk.commands = $$AWK -f propname.awk ../incl/TAXI/propdef.h > PropName.cpp
      QMAKE_EXTRA_TARGETS += awk
      #PRE_TARGETDEPS =  PropName.cpp
}


build_pass:CONFIG(debug, debug|release) {
    message(Debug: configuring $$TARGET version:$$VERSION)
    } else:build_pass{
    message(Release: configuring $$TARGET version:$$VERSION)
}

message(configuring the library $$TARGET (static) version:$$VERSION ($$OBJECTS_DIR))

