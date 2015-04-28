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
\file gprop.h
\brief  Display properties of a graph, enable/disable menus
*/

#ifndef GRAPH_PROPERTIES_H
#define GRAPH_PROPERTIES_H

#include <QT/Action_def.h>
#include <TAXI/Tsvector.h>
#include <TAXI/graph.h>

#include <QRadioButton>
#include <QLineEdit>
#include <QMenuBar>

class pigaleWindow;

class RoRadioButton : public QRadioButton
{ 
  Q_OBJECT
public:
  RoRadioButton(QWidget * parent);
  ~RoRadioButton(){};
  void mousePressEvent(QMouseEvent* e);
  void mouseReleaseEvent(QMouseEvent* e);
};

class Graph_Properties : public QWidget
{ 
  Q_OBJECT

public:
  Graph_Properties(QWidget* parent,QMenuBar* menu,pigaleWindow* mw);
  ~Graph_Properties();
 
  void update(GraphContainer & GC,bool print = true);
  void updateMenu(bool val){_updateMenu = val;}
  bool Simple(){return S;}
  bool Planar(){return P;}
  bool Triangulation(){return T;}
  bool Acyclic(){return A;}
  bool Biparti(){return B;}
  bool MaxBiparti(){return MaxBi;}
  bool Con1(){return C1;}
  bool Con2(){return C2;}
  bool Con3(){return C3;}
  bool OuterPlanar(){return Outer;}
  bool SeriePlanar(){return Serie;}
  bool Regular(){return R;}
  int DegreeMin(){return dmin;}
  int DegreeMax(){return dmax;}
  int NumSources(){return ns;}
  int NumSinks(){return nt;}
  bool actionAllowed(int action){return  allow[action];}

  QLineEdit* LE_N,*LE_M,*LE_Min,*LE_Max;
  RoRadioButton* RBConnected;
  RoRadioButton* RB2Connected;
  RoRadioButton* RB3Connected;
  RoRadioButton* RBPlanar;
  RoRadioButton* RBMxPlanar;
  RoRadioButton* RBOuPlanar;
  RoRadioButton* RBSerie;
  RoRadioButton* RBSimple;
  RoRadioButton* RBBipartite;
  RoRadioButton* RBAcyclic;

private:
  pigaleWindow * mw; 
  QMenuBar *menu;
  svector<bool> allow;
  bool _updateMenu;
  int ns,nt,dmin,dmax;
  bool S,P,A,B,R,C1,C2,C3,T,Outer,Serie,MaxBi;
  void allowAction(int action,bool condition);
};

#endif // GRAPH_PROPERTIES_H
