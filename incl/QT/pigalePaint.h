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

#ifndef MYPAINT_H
#define MYPAINT_H

#include <TAXI/graphs.h> 

class pigaleWindow;
class QPrinter;

/*! \file 
\brief To display non Fary drawings
 */
/*! \class  pigalePaint
\brief To display non Fary drawings

This class is used for plane drawings of graphs which are not Fary drawings.
Fary drawings uses the editor to be displayed.<br>
For each drawing algorithm, one has to write  a function taking as arguments a pointer on a QPainter and a pointer on this class. <b>pigalePaint</b> uses a pointer to such a function to display the drawing..<br>
  It works using  a copy of the graph.
*/
class pigalePaint : public QWidget 
{
  //Q_OBJECT

public:
  pigalePaint(QWidget *parent=0,pigaleWindow *father = 0);
  ~pigalePaint();
  int to_x(double x);   
  int to_y(double y);   
  void DrawSeg(QPainter *p,Tpoint &a,Tpoint &b,int col,int width=1);
  void DrawRect(QPainter *p,Tpoint &a,double nx,double ny,int col);
  void DrawText(QPainter *p,double x,double y,QString txt);
  void DrawText(QPainter *p,Tpoint &a,tvertex v,int col,int center);
  void DrawText(QPainter *p,double x,double y,double nx,double ny,tvertex v,int color);
  void DrawTriangle(QPainter *p,Tpoint &p1,Tpoint &p2,Tpoint &p3,int col);
  void update(int index,bool NewDrawing = true);
  void update();
  void print(QPrinter *printer);
  void image(QPrinter* printer, QString suffix);

private:
  void drawIt(QPainter *);
  void paintEvent(QPaintEvent *);
  void showEvent(QShowEvent*);
  void hideEvent(QHideEvent*);
  void resizeEvent(QResizeEvent* e);
  void mousePressEvent(QMouseEvent *e);
  void mouseMoveEvent(QMouseEvent *e);
  void wheelEvent(QWheelEvent *e);
  void keyPressEvent(QKeyEvent *k);

private:
  pigaleWindow *mw;
  bool isHidden;
  //QPrinter* printer;
  int index;
  double zoom;
  QPoint posClick;

public:
  GraphContainer GCP;
  double Wx_min,Wx_max,Wy_min,Wy_max;
  double xscale,xtr,yscale,ytr,xtr0,ytr0,xscale0,yscale0;
  double xmin,xmax,ymin,ymax;
};
#endif
