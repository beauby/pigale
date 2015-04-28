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
\file SymWindow.h
\brief Display and compute the symetries of a graph
*/


#ifndef _SYMWINDOW_H_INCLUDED_
#define _SYMWINDOW_H_INCLUDED_

#include <TAXI/graphs.h>
#include <TAXI/Tmessage.h>
#include <TAXI/Tpoint.h>
#include <TAXI/color.h>

// in HeapSort.cpp
void HeapSort(int (*f)(int a,int b),int first,int nelements,int *heap);
// in Debug.cpp
bool& debug();

QColor OppCol(QColor & col);
static bool Equal(double x, double y);
static bool Less(double x, double y);
static bool Null(double x);
static int FindSymetry(GeometricGraph &G,int i1,int i2,int i3,int rotate
		   ,int &confusion,int &identify,int &NumFixPoints);
static void SymColorEdges(GeometricGraph &G);
static int RemoveConfusion(GeometricGraph &G,int *heap,int i0,int i1,int &label);
static bool MatchZ(svector<int> & negatif,int kpos,int npos,int &kneg);
static void FillCoords(GeometricGraph &G,double *x,int i1);
static void FillCoords(GeometricGraph &G,int i1,int i2,int i3);
static void RestaureCoords(GeometricGraph &G);
static void Rotate(GeometricGraph &G,int rotate);		
static int CheckSymmetry(GeometricGraph &G,int MaxLabel);
static int RemoveFixedPoints(GeometricGraph &G,int *heap);

static double const epsilon = 1.E-6;
static double *xcoord,*ycoord,*zcoord;
//static double *xGcoord,*yGcoord;
static double *angles;
static double theta_12,theta_23;
static int match0,match1;
static bool Opt;

class SymWindow : public QWidget
{

public:
  SymWindow(GraphSymPrivate *g,QWidget * parent);
  ~SymWindow(){};
  void initialize();
  void FindSym();
  void DrawSym();
  void Axes(int i1,int i2);
  void print(QPrinter *printer);
  void image(QPrinter* printer, QString suffix);
private:
  void paintEvent(QPaintEvent *e);
  void showEvent(QShowEvent*);
  void hideEvent(QHideEvent*);
  void resizeEvent(QResizeEvent* e);
  void Normalise();
  void update(QPainter *p);

public:
  int sym;
  int start,start0;
  
private:
  GraphSymPrivate *gsp;
  bool is_init;
  bool isHidden;
  double xmul,xtr,ymul,ytr;
  int fs;//fontsize
  int axe1,axe2;
};

#endif

