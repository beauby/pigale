/****************************************************************************
**
** Copyright (C) 2001 Hubert de Fraysseix, Patrice Ossona de Mendez.
** All rights reserved.
** This file is part of the PIGALE Toolkit.
**
** This file may be distributed under the terms of the GNU Public License
** appearing in the file LICENSE.HTML included in the packaging of this file.
**
*****************************************************************************/

/*!
\file Pigale.h
\brief Prototypes of non-member functions defined in tgraph
*/

#ifndef PIGALE_H
#define PIGALE_H
#include <TAXI/Tbase.h> 
#include <TAXI/Tmessage.h> 
#include <TAXI/color.h> 
#include <TAXI/graphs.h>
#include <TAXI/Tfile.h>
#include <TAXI/random.h>

int NPBipolar(TopologicalGraph &G, tbrin bst);
int EmbedGVision(TopologicalGraph &G);
int TestOuterPlanar(TopologicalGraph &G);
void ColorPoles(GeometricGraph &G);
bool CheckCoordNotOverlap(GeometricGraph & G);
void ComputeGeometricCir(GeometricGraph &G,svector<tbrin> &cir);
void BFSOrientTree(TopologicalGraph &G, tvertex v0);

// Drawings
int EmbedTContact(TopologicalGraph &G,bool schnyderLongestFace);
int EmbedTriangle(TopologicalGraph &G);
int EmbedFPPRecti(TopologicalGraph &G,bool schnyderLongestFace);
int EmbedFPP(TopologicalGraph &G,bool schnyderRect,bool schnyderLongestFace);
int EmbedCCD(TopologicalGraph &G,bool compact);
int EmbedTutteCircle(TopologicalGraph &G,bool schnyderLongestFace);
int FindNPSet(TopologicalGraph &);
int split(Graph &G0,int &NumberOfClasses,int usedDistance);
int EmbedVision(TopologicalGraph &G);
//int Vision(TopologicalGraph &G);
int Vision(TopologicalGraph &xG,int morg);
int EmbedContactBip(GeometricGraph &G);
int DecompMaxBip(TopologicalGraph &G);
int Polar(TopologicalGraph &G);
int Embed3d(TopologicalGraph &G0,int usedDistance);
int Embed3dbis(TopologicalGraph &G0);
int Embed3dSchnyder(TopologicalGraph &G0,bool schnyderLongestFace);
int EmbedPolyline(TopologicalGraph &G);
int EmbedCurves(TopologicalGraph &G);
int EmbedPolrecDFS(TopologicalGraph &G);
int EmbedPolrecBFS(TopologicalGraph &G);
int EmbedPolrecLR(TopologicalGraph &G);

// Generators
//in Generate.cpp
GraphContainer *GenerateGrid(int a, int b);
GraphContainer *GenerateCompleteGraph(int a);
GraphContainer *GenerateCompleteBiGraph(int a,int b);
GraphContainer *GenerateRandomGraph(int a,int b,bool randomEraseMultipleEdges);
GraphContainer *GenerateRandomGraph(int a,int b);

//in SchaefferGen.cpp
GraphContainer *GenerateSchaeffer(int n_ask,int type,int e_connectivity
                                  ,bool randomEraseMultipleEdges=false,bool randomUseGeneratedCir=false);

// Outerplanar maps
GraphContainer *GenerateRandomOuterplanarGraph(int n,bool randomEraseMultipleEdges);
GraphContainer *GenerateRandomOuterplanarGraph(int n,int m,bool randomEraseMultipleEdges);

// random
void shuffleCir(TopologicalGraph &G);


#endif
