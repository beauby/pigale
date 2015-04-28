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


#include "pigaleWindow.h"
#include <QT/pigalePaint.h>
#include <QT/pigaleQcolors.h>
#include <QT/Misc.h>

#include <QPixmap>
#include <QPainter>
#include <QSvgGenerator>
#include <QPrinter>


/*! \file 
\brief To display non Fary drawings
 */
void DrawPolrec(QPainter *p,pigalePaint *paint)
  {TopologicalGraph G(paint->GCP);
  Prop1<tstring> title(G.Set(),PROP_TITRE);
  Prop1<Tpoint> pmin(G.Set(),PROP_POINT_MIN);
  Prop<Tpoint> p1(G.Set(tvertex()),PROP_DRAW_POINT_1);
  Prop<Tpoint> p2(G.Set(tvertex()),PROP_DRAW_POINT_2);
  Prop<double> x1(G.Set(tedge()),PROP_DRAW_DBLE_1 );
  Prop<double> x2(G.Set(tedge()),PROP_DRAW_DBLE_2 );
  Prop<double> y1(G.Set(tedge()),PROP_DRAW_DBLE_3 );
  Prop<double> y2(G.Set(tedge()),PROP_DRAW_DBLE_4);
  Prop<double> y(G.Set(tedge()),PROP_DRAW_DBLE_5);
  Prop<short> ecolor(G.Set(tedge()),PROP_COLOR);
  Prop<short> vcolor(G.Set(tvertex()),PROP_COLOR);
  Prop<bool> isTree(G.Set(tedge()),PROP_ISTREE); 
  Prop<int> elabel(G.Set(tedge()),PROP_LABEL); 
  Prop<int> ewidth(G.Set(tedge()),PROP_WIDTH);

  bool drawTextEdges = (G.ne() < 100);
  QString stitle(~title());
  if(drawTextEdges)paint->DrawText(p,pmin().x(),pmin().y(),stitle);
  
  // draw vertices
  for(tvertex v = 1;v <= G.nv();v++)
      {double dx = (p2[v].x() - p1[v].x()) ;   
      double x = p1[v].x() ;     
      double y = p1[v].y(); 
      paint->DrawText(p,x,y, dx,1.,v,vcolor[v]);
      }
  // draw edges
  Tpoint e1,e2,e3,e4;
   for(tedge e = 1;e <= G.ne();e++)
       {if(isTree[e])
           {e1 = Tpoint(x1[e],y1[e]);
           e2 = Tpoint(x1[e],y2[e]);
           paint->DrawSeg(p,e1,e2,ecolor[e],ewidth[e]);
           }
       else // cotree edges   (x1,y1) -> (x1,y) -> (x2,y) -> (x2,y2)
           {e1 = Tpoint(x1[e],y1[e]);
           e2 = Tpoint(x1[e],y[e]);
           e3 = Tpoint(x2[e],y[e]);
           e4 = Tpoint(x2[e],y2[e]);
           paint->DrawSeg(p,e1,e2,ecolor[e],ewidth[e]);
           paint->DrawSeg(p,e2,e3,ecolor[e],ewidth[e]);
           paint->DrawSeg(p,e3,e4,ecolor[e],ewidth[e]);
           if(drawTextEdges)
               {QString label=QString("%1").arg(elabel[e]);
               // text is drawn at  position of lower edge occu
               paint->DrawText(p,x1[e],y[e],label);
               }
           }
       }
  }
void DrawPolar(QPainter *p,pigalePaint *paint)
  {TopologicalGraph G(paint->GCP);
  Prop<Tpoint> Vcoord(G.Set(tvertex()),PROP_DRAW_COORD);
  Prop<Tpoint> Epoint1(G.Set(tedge()),PROP_DRAW_POINT_1);
  Prop<Tpoint> Epoint2(G.Set(tedge()),PROP_DRAW_POINT_2);
  Prop<Tpoint> Epoint11(G.Set(tedge()),PROP_DRAW_POINT_3);
  Prop<Tpoint> Epoint12(G.Set(tedge()),PROP_DRAW_POINT_4);
  Prop<Tpoint> Epoint21(G.Set(tedge()),PROP_DRAW_POINT_5);
  Prop<Tpoint> Epoint22(G.Set(tedge()),PROP_DRAW_POINT_6);
  Prop<double> Erho   (G.Set(tedge()),PROP_DRAW_DBLE_1);
  Prop<double> Etheta1(G.Set(tedge()),PROP_DRAW_DBLE_2);
  Prop<double> Etheta2(G.Set(tedge()),PROP_DRAW_DBLE_3);
  Prop1<double> nw(G.Set(),PROP_DRAW_DBLE_1);
  Prop<short> vcolor(G.Set(tvertex()),PROP_COLOR);
  int m = G.ne(); 
  int ox,oy,nx,ny,theta,dt;
  QPen pn = p->pen();pn.setWidth(1);

  for (tedge ee=1; ee<=m; ee++)
      {if (Erho[ee]==-1)
          {paint->DrawSeg(p,Epoint1[ee],Epoint2[ee],Red);
          continue;
          }
      if (Epoint11[ee]!=Tpoint(-1,-1))
          {paint->DrawSeg(p,Epoint1[ee],Epoint11[ee],Blue);
          paint->DrawSeg(p,Epoint11[ee],Epoint12[ee],Blue);
          }
      else if (Epoint12[ee]!=Tpoint(-1,-1))
          paint->DrawSeg(p,Epoint1[ee],Epoint12[ee],Blue);
      if (Epoint21[ee]!=Tpoint(-1,-1))
          {paint->DrawSeg(p,Epoint2[ee],Epoint21[ee],Blue);
          paint->DrawSeg(p,Epoint21[ee],Epoint22[ee],Blue);
          }
      else if (Epoint22[ee]!=Tpoint(-1,-1))
          paint->DrawSeg(p,Epoint2[ee],Epoint22[ee],Blue);
	  
      pn.setColor(color[Blue]);pn.setWidth(2);p->setPen(pn);
      ox = paint->to_x(-Erho[ee]);
      oy = paint->to_y(Erho[ee]);
      nx = (int)(2*Erho[ee]*paint->xscale + .5);
      ny = (int)(2*Erho[ee]*paint->yscale + .5);
      theta = (int)(Etheta1[ee]*16*180/PI+.5);
      dt = (int)((Etheta2[ee] - Etheta1[ee])*16*180/PI+.5); 
      p->drawArc(ox,oy,nx,ny,theta,dt);
      }
  
  // Draw the vertices
  double dx = .001;
  for(tvertex v = 1;v <= G.nv();v++)
      {double x = Vcoord[v].x()-dx*.5;
      double y = Vcoord[v].y()-dx*.5;
      Tpoint pt(x,y);
      paint->DrawText(p,pt, v,vcolor[v],1);
      }
  }
void DrawPolyline(QPainter *p,pigalePaint *paint)
  {GeometricGraph G(paint->GCP);
  Prop<Tpoint> Vcoord(G.Set(tvertex()),PROP_DRAW_POINT_1);
  Prop<Tpoint> Epoint1(G.Set(tedge()),PROP_DRAW_POINT_1);
  Prop<Tpoint> Epoint2(G.Set(tedge()),PROP_DRAW_POINT_2);
  Prop<Tpoint> Ebend(G.Set(tedge()),PROP_DRAW_POINT_3);
  Prop<short> ecolor(G.Set(tedge()),PROP_COLOR);
  Prop<short> vcolor(G.Set(tvertex()),PROP_COLOR);

  QPen pn = p->pen();pn.setWidth(2);

  for (tedge ee=1; ee<= G.ne(); ee++)
      {if (Ebend[ee] != Tpoint(-1, -1)) 
          {paint->DrawSeg(p, Epoint1[ee], Ebend[ee],ecolor[ee]);
          paint->DrawSeg(p, Ebend[ee], Epoint2[ee],ecolor[ee]);
          }
      else
          paint->DrawSeg(p, Epoint1[ee], Epoint2[ee] , ecolor[ee]);
      }
  // Draw the vertices
  p->setFont(QFont("sans",Min((int)(.45 * Min(paint->xscale,paint->yscale) + .5),13)));
  for(tvertex v = 1;v <= G.nv();v++)
       paint->DrawText(p,Vcoord[v],v,vcolor[v],1);
  }

void DrawCurves(QPainter *p,pigalePaint *paint)
  {GeometricGraph G(paint->GCP);
  Prop<Tpoint> Epoint1(G.Set(tedge()),PROP_DRAW_POINT_1);
  Prop<Tpoint> Epoint2(G.Set(tedge()),PROP_DRAW_POINT_2);
  Prop<Tpoint> Epoint3(G.Set(tedge()),PROP_DRAW_POINT_3);
  Prop<Tpoint> vcoord(G.Set(tvertex()),PROP_DRAW_POINT_1);
  Prop<short> ecolor(G.Set(tedge()),PROP_COLOR);
  Prop<short> vcolor(G.Set(tvertex()),PROP_COLOR);
  int m = G.ne(); 
  int n = G.nv(); 
  QPen pn = p->pen();pn.setWidth(2);
  QVector<QPoint>  bez(7);

  for (tedge ee=1; ee<=m; ee++)
    {if (Epoint2[ee]!=Tpoint(0,0)) {
       bez[0] = QPoint(paint->to_x(vcoord[G.vin[ee]].x()),paint->to_y(vcoord[G.vin[ee]].y()));
       bez[1] = QPoint(paint->to_x(Epoint1[ee].x()),paint->to_y(Epoint1[ee].y()));
       bez[2] = QPoint(paint->to_x(Epoint1[ee].x()),paint->to_y(Epoint1[ee].y()));
       bez[3] = QPoint(paint->to_x(Epoint2[ee].x()),paint->to_y(Epoint2[ee].y()));
       bez[4] = QPoint(paint->to_x(Epoint3[ee].x()),paint->to_y(Epoint3[ee].y()));
       bez[5] = QPoint(paint->to_x(Epoint3[ee].x()),paint->to_y(Epoint3[ee].y()));
       bez[6] = QPoint(paint->to_x(vcoord[G.vin[-ee]].x()),paint->to_y(vcoord[G.vin[-ee]].y()));
       QPainterPath path;
       path.moveTo(bez.at(0));
       path.cubicTo(bez.at(1),bez.at(2),bez.at(3));
       p->strokePath(path, p->pen());
       path.moveTo(bez.at(3));
       path.cubicTo(bez.at(4),bez.at(5),bez.at(6));
       p->strokePath(path, p->pen());
       //paint->DrawRect(p,Epoint1[ee],3,3,Red);
       //paint->DrawRect(p,Epoint3[ee],3,3,Red);
     }
    else
      paint->DrawSeg(p, vcoord[G.vin[ee]], vcoord[G.vin[-ee]] , ecolor[ee]);
      }
  // Draw the vertices
  p->setFont(QFont("sans",Min((int)(10*Min(paint->xscale,paint->yscale) + .5),13)));
  for(tvertex v = 1;v <= n;v++) 
      paint->DrawText(p,vcoord[v],v,vcolor[v],1);
  }

void DrawTContact(QPainter *p,pigalePaint *paint)
  {GeometricGraph G(paint->GCP);
  Prop<Tpoint> hp1(G.Set(tvertex()),PROP_DRAW_POINT_1);
  Prop<Tpoint> hp2(G.Set(tvertex()),PROP_DRAW_POINT_2);
  Prop<Tpoint> vp1(G.Set(tvertex()),PROP_DRAW_POINT_3);
  Prop<Tpoint> vp2(G.Set(tvertex()),PROP_DRAW_POINT_4);
  Prop<Tpoint> postxt(G.Set(tvertex()),PROP_DRAW_POINT_5);
  Prop1<double> sizetext(G.Set(),PROP_DRAW_DBLE_1);
  tvertex v;	
  // Draw horizontals and verticals
  for(v = 1;v <= G.nv();v++)
      {if(hp1[v].x() > .0)paint->DrawSeg(p,hp1[v],hp2[v],Black);
      if(vp1[v].x() > .0)paint->DrawSeg(p,vp1[v],vp2[v],Black);
      }
  // Draw text
  p->setFont(QFont("sans",Min((int)(sizetext() * Min(paint->xscale,paint->yscale) + .5),13)));
  for(v=1; v <= G.nv();v++)
      paint->DrawText(p,postxt[v],v,G.vcolor[v],0);
  }
void DrawBip2Pages(QPainter *p,pigalePaint *paint)
  {GraphContainer GC = paint->GCP;
  GeometricGraph G(paint->GCP);
  Prop<int> h(G.Set(tvertex()),PROP_DRAW_INT_1);
  Prop<int> h1(G.Set(tvertex()),PROP_DRAW_INT_2);
  Prop<int> h2(G.Set(tvertex()),PROP_DRAW_INT_3);
  Prop1<Tpoint> pmax(G.Set(),PROP_POINT_MAX);
  Prop1<Tpoint> pmin(G.Set(),PROP_POINT_MIN);

  QPen pn = p->pen();pn.setWidth(1);
 
  QPoint ps,pt,ps2,pt2;
  // horizontale
  int yh = pmin().y()+ pmax().y()/2;
  ps = QPoint(paint->to_x(pmin().x()),paint->to_y(yh));
  pt = QPoint(paint->to_x(pmax().x()),paint->to_y(yh));
  pn.setWidth(1);pn.setColor(color[Grey1]); p->setPen(pn);
  p->drawLine(ps,pt);

  pn.setColor(color[Black]);
  pn.setWidth(1);p->setPen(pn);
  // draw edges: 2 segments
  double x1,x2,dh;
  tvertex vmin,vmax;
  for(tedge e = 1; e < G.ne();e++)
      {tvertex v1 = G.vin[e];
      tvertex v2 = G.vin[-e];
      if(h[v1] <h[v2])
          {vmin = v1;vmax = v2;}
      else
          {vmin = v2;vmax = v1;}

      x1 = (double)Min(h[v1],h[v2]);
      x2 = (double)Max(h[v1],h[v2]);
      dh = Min((x2-x1)*paint->xscale,(x2-x1)*paint->yscale)/2;
      QRect r = QRect(paint->to_x(x1),paint->to_y(yh)-dh/2,(int)((x2-x1)*paint->xscale),(int)dh);
      if(G.vcolor[vmax] == Red)
          p->drawArc(r,0,180*16);
       else
           p->drawArc(r,0,-180*16);
      }
  // Draw verticces
  QBrush pb = p->brush();
  pb.setStyle(Qt::SolidPattern);
  pb.setColor(color[White]);
  p->setBrush(pb);
  int dy = Min(10,paint->height()/(pmax().y()+1)-2);
  QFont font = QFont("sans",dy);
  p->setFont(font);
  pn.setWidth(1);
  for(tvertex v = 1;v <= G.nv();v++)
      {ps = QPoint(paint->to_x(h[v]),paint->to_y(yh));
      QString t = getVertexLabel(GC,v);
      QSize size = QFontMetrics(font).size(Qt::AlignCenter,t);
      int dx =size.width() + 2;   dy =size.height();// + 2;
      if(t.length() == 0)dx = dy= 8;
      QRect rect = QRect(ps.x()-dx/2 ,ps.y()-dy/2,dx,dy);
      pn.setColor(color[G.vcolor[v]]);p->setPen(pn);
      p->drawRect(rect);
      pn.setColor(color[Black]);p->setPen(pn);
      p->drawText(rect,Qt::AlignCenter,t);
      }
  }

void DrawBipContact(QPainter *p,pigalePaint *paint)
  {GraphContainer GC = paint->GCP;
  GeometricGraph G(paint->GCP);
  Prop<int> h(G.Set(tvertex()),PROP_DRAW_INT_1);
  Prop<int> h1(G.Set(tvertex()),PROP_DRAW_INT_2);
  Prop<int> h2(G.Set(tvertex()),PROP_DRAW_INT_3);
  Prop1<Tpoint> pmax(G.Set(),PROP_POINT_MAX);
  Prop1<Tpoint> pmin(G.Set(),PROP_POINT_MIN);

  QPen pn = p->pen();pn.setWidth(1);
  QBrush pb = p->brush();
  pb.setStyle(Qt::SolidPattern);
 
  QPoint ps,pt,ps2,pt2;

  // Drawing the diagonal
  ps = QPoint(paint->to_x(pmin().x()),paint->to_y(pmin().y()));
  pt = QPoint(paint->to_x(pmax().x()),paint->to_y(pmax().y()));
  pn.setColor(color[Grey1]); p->setPen(pn);
  p->drawLine(ps,pt);

  // Drawing the vertices: horizontal and vertical segments
  for(tvertex v = 1;v <= G.nv();v++)
      {double delta = (h1[v] < h[v])?.45:-.45;
      pn.setColor(color[G.vcolor[v]]);
      pn.setWidth(1);p->setPen(pn);
      if(G.vcolor[v] == Red)//horizontales
          {ps = QPoint(paint->to_x(h1[v]),paint->to_y(h[v]));
          pt  = QPoint(paint->to_x(h2[v]),paint->to_y(h[v]));
          if(h2[v] > h[v]  && h1[v] > h[v])
              {ps2 = QPoint(paint->to_x(h[v]),paint->to_y(h[v]));
              p->drawLine(ps2,pt);
              }
          else if(h2[v] < h[v]  && h1[v] < h[v])
              {pt2 = QPoint(paint->to_x(h[v]),paint->to_y(h[v]));
              p->drawLine(ps,pt2);
              }
          if(h1[v] == h2[v])// isthme
              {ps = QPoint(paint->to_x(h1[v]),paint->to_y(h[v]));
              pt  = QPoint(paint->to_x(h2[v]+delta),paint->to_y(h[v]));
              }
          pn.setWidth(2);p->setPen(pn);
          p->drawLine(ps,pt);
          }
      else  // verticales
          {ps = QPoint(paint->to_x(h[v]),paint->to_y(h1[v]));
          pt  = QPoint(paint->to_x(h[v]),paint->to_y(h2[v]));
          if(h2[v] > h[v]  && h1[v] > h[v])
              {ps2 = QPoint(paint->to_x(h[v]),paint->to_y(h[v]));
              p->drawLine(ps2,pt);
              }
          else if(h2[v] < h[v]  && h1[v] < h[v])
              {pt2 = QPoint(paint->to_x(h[v]),paint->to_y(h[v]));
              p->drawLine(ps,pt2);
              }
          if(h1[v] == h2[v])// isthme
              {ps = QPoint(paint->to_x(h[v]),paint->to_y(h1[v]));
              pt  = QPoint(paint->to_x(h[v]),paint->to_y(h2[v]+delta));
              } 
          pn.setWidth(2);p->setPen(pn);
          p->drawLine(ps,pt);
          }
      }
  //drawing labels
  int dy = Min(12,paint->height()/(pmax().y()+1)-2);
  QFont font = QFont("sans",dy);
  p->setFont(font);
  pb.setColor(color[White]);
  p->setBrush(pb);
  pn.setWidth(1);
  for(tvertex v = 1;v <= G.nv();v++) //trace des labels
      {double delta = (h1[v] < h[v])?.9:-.9;
      if(G.vcolor[v] == Red)
          {if(h1[v] != h2[v])
              {ps = QPoint(paint->to_x(h1[v]),paint->to_y(h[v]));
              pt  = QPoint(paint->to_x(h2[v]),paint->to_y(h[v]));
              }
          else
              {ps = QPoint(paint->to_x(h1[v]),paint->to_y(h[v]));
              pt  = QPoint(paint->to_x(h2[v]+delta),paint->to_y(h[v]));
              }
          }
      else
          {if(h1[v] != h2[v])
              {ps = QPoint(paint->to_x(h[v]),paint->to_y(h1[v]));
              pt  = QPoint(paint->to_x(h[v]),paint->to_y(h2[v]));
              }
          else
              {ps = QPoint(paint->to_x(h[v]),paint->to_y(h1[v]));
              pt  = QPoint(paint->to_x(h[v]),paint->to_y(h2[v]+delta));
              } 
          }
      QString t = getVertexLabel(GC,v);
      QSize size = QFontMetrics(font).size(Qt::AlignCenter,t);
      int dx =size.width() + 2;   dy =size.height() + 2;
      if(t.length() == 0)dx = dy= 8;
      QRect rect = QRect((ps.x() + pt.x() - dx)/2,(ps.y() + pt.y() - dy)/2,dx,dy);
      pn.setColor(color[G.vcolor[v]]);p->setPen(pn);
      p->drawRect(rect);
      pn.setColor(color[Black]);p->setPen(pn);
      p->drawText(rect,Qt::AlignCenter,t);
      }
  }
void DrawFPPVisibility(QPainter *p,pigalePaint *paint)
  {TopologicalGraph G(paint->GCP);
  Prop<int> xliv(G.Set(tvertex()),PROP_DRAW_INT_1);
  Prop<int> xriv(G.Set(tvertex()),PROP_DRAW_INT_2);
  Prop<int> y(G.Set(tvertex()),PROP_DRAW_INT_3);
  Prop<int> xje(G.Set(tedge()),PROP_DRAW_INT_4);
  Prop<short> vcolor(G.Set(tvertex()),PROP_COLOR);
  Prop<short> ecolor(G.Set(tedge()),PROP_COLOR);

  // Draw edges
  int h1,h2,x1,x2;
  double alpha = .5;
  double beta  = .825;
  Tpoint a,b;
  for(tedge e=1; e <= G.ne();e++)
      {h1 = y[G.vin[e]]; 
       h2 = y[G.vin[-e]];
       x1 = xriv[G.vin[e]]; 
       x2 = xliv[G.vin[-e]];
      if(h1 < h2)
          {a.x() = b.x() = xje[e]; 
          a.y() = h1;  
          b.y() = h2 - 2*alpha;
          paint->DrawSeg(p,a,b,ecolor[e]);
          }
      else if(h1 > h2)
          {a.x() = b.x() = xje[e];
          a.y() = h2;
          b.y() = h1 - 2*alpha;
          paint->DrawSeg(p,a,b,ecolor[e]);
          }
      else if(x1 < x2)
          {a.x() = x1 + beta;
          b.x() = x2 - beta;
          a.y() = b.y() = h1 -alpha;
          paint->DrawSeg(p,a,b,ecolor[e]);
          }
      else if(x1 > x2)
          {a.x() = xriv[G.vin[-e]] + beta;  
          b.x() = xliv[G.vin[e]]  - beta;
          a.y() = b.y() = h1 -alpha;
          paint->DrawSeg(p,a,b,ecolor[e]);
          }
      }

  // Draw vertices
  double xt = .9*Min(2*alpha*paint->xscale,beta*paint->yscale);
  p->setFont(QFont("sans",Min((int)(xt + .5),13)));
  for(tvertex v=1; v <= G.nv();v++) 
      paint->DrawText(p,xliv[v]-beta, y[v], xriv[v]-xliv[v]+2.*beta, 2.*alpha,v,vcolor[v]);
  }
void DrawVisibility(QPainter *p,pigalePaint *paint)
  {TopologicalGraph G(paint->GCP);
  Prop<Tpoint> P1(G.Set(tedge()),PROP_DRAW_POINT_1);
  Prop<Tpoint> P2(G.Set(tedge()),PROP_DRAW_POINT_2);
  Prop<int> x1(G.Set(tvertex()),PROP_DRAW_INT_1);
  Prop<int> x2(G.Set(tvertex()),PROP_DRAW_INT_2);
  Prop<int> y(G.Set(tvertex()),PROP_DRAW_INT_5);
  Prop<short> ecolor(G.Set(tedge()),PROP_COLOR);
  Prop<short> vcolor(G.Set(tvertex()),PROP_COLOR);

  double alpha=0.35;
  p->setFont(QFont("sans",Min((int)(1.8*alpha * Min(paint->xscale,paint->yscale) + .5),13)));
  Tpoint a,b;
  for(tvertex v=1;v<=G.nv();v++)
      paint->DrawText(p,x1[v]-alpha,y[v]+alpha, x2[v]-x1[v]+2*alpha,2*alpha,v,vcolor[v]);
  for (tedge e = 1;e <= G.ne();e++)
      {a.x() = P1[e].x(); a.y() = P1[e].y() + alpha;
      b.x() = P1[e].x();  b.y() = P2[e].y() - alpha;
      paint->DrawSeg(p,a,b,ecolor[e]);
      }
  }
void DrawGeneralVisibility(QPainter *p,pigalePaint *paint)
  {TopologicalGraph G(paint->GCP);
  Prop<Tpoint> P1(G.Set(tedge()),PROP_DRAW_POINT_1);
  Prop<Tpoint> P2(G.Set(tedge()),PROP_DRAW_POINT_2);
  Prop<int> x1(G.Set(tvertex()),PROP_DRAW_INT_1);
  Prop<int> x2(G.Set(tvertex()),PROP_DRAW_INT_2);
  Prop<int> x1m(G.Set(tvertex()),PROP_DRAW_INT_3);
  Prop<int> x2m(G.Set(tvertex()),PROP_DRAW_INT_4);
  Prop<int> y(G.Set(tvertex()),PROP_DRAW_INT_5);
  Prop<short> ecolor(G.Set(tedge()),PROP_COLOR);
  Prop<short> vcolor(G.Set(tvertex()),PROP_COLOR);
  Prop<int> ewidth(G.Set(tedge()),PROP_WIDTH,1);
  double alpha=0.35;
  Tpoint a,b;


  p->setFont(QFont("sans",Min((int)(1.8*alpha * Min(paint->xscale,paint->yscale) + .5),13)));
  for(tvertex v=1;v<=G.nv();v++)
      {if(x1m[v] != x2m[v]) // always 
          {a.x() = x1m[v]; a.y() = y[v];
          b.x() = x2m[v];  b.y() = y[v];
          paint->DrawSeg(p,a,b,Black);
          }
      paint->DrawText(p,x1[v]-alpha,y[v]+alpha, x2[v]-x1[v]+2*alpha,2*alpha,v,vcolor[v]);
      }
  for (tedge e = 1;e <= G.ne();e++)
    {a.x() = P1[e].x(); a.y() = P1[e].y();
    if (a.x()>=x1[G.vin[e]] && a.x()<=x2[G.vin[e]]) a.y()+=alpha;
    b.x() = P1[e].x();  b.y() = P2[e].y();
    if (b.x()>=x1[G.vin[-e]] && b.x()<=x2[G.vin[-e]]) b.y()-=alpha;
    paint->DrawSeg(p,a,b,ecolor[e],ewidth[e]);
    }
  }
void DrawTriangle(QPainter *p,pigalePaint *paint)
  {TopologicalGraph G(paint->GCP);
  Prop1<Tpoint> pmin(G.Set(),PROP_POINT_MIN);
  Prop1<Tpoint> pmax(G.Set(),PROP_POINT_MAX);
  Prop<Tpoint> pleft(G.Set(tvertex()),PROP_DRAW_POINT_1);
  Prop<Tpoint> pright(G.Set(tvertex()),PROP_DRAW_POINT_2);
  Prop<Tpoint> ptop(G.Set(tvertex()),PROP_DRAW_POINT_3);
  Prop<short> vcolor(G.Set(tvertex()),PROP_COLOR);

  p->setFont(QFont("sans",Min((int)(Min(paint->xscale,paint->yscale) + .5),13)));
  for(tvertex iv = 1; iv <= G.nv();iv++)
      {paint->DrawTriangle(p,pleft[iv],pright[iv],ptop[iv],vcolor[iv]);
      Tpoint center = (pleft[iv]+pright[iv]+ptop[iv])/3.;
      paint->DrawText(p,center,iv,vcolor[iv],1);
      }

  }

//**********************************************************************************************
//**********************************************************************************************

typedef void (*draw_func)(QPainter *p,pigalePaint *paint);
struct DrawThing {
    draw_func	 f;
    const char	*name;
};

static DrawThing DrawFunctions[] = 
    {
    {DrawVisibility,QT_TRANSLATE_NOOP("pigalePaint","Visibility")},//0
    {DrawFPPVisibility,QT_TRANSLATE_NOOP("pigalePaint","FPP Visibility")},//1
    {DrawGeneralVisibility,QT_TRANSLATE_NOOP("pigalePaint","General Visibility")},//2
    {DrawBipContact,QT_TRANSLATE_NOOP("pigalePaint","Contact")},//3
    {DrawPolar,QT_TRANSLATE_NOOP("pigalePaint","Polar")},//4
    {DrawTContact,QT_TRANSLATE_NOOP("pigalePaint","T Contact")},//5 
    {DrawPolyline,QT_TRANSLATE_NOOP("pigalePaint", "Polyline")},//6
    {DrawCurves,QT_TRANSLATE_NOOP("pigalePaint", "Curves")},//7
    {DrawPolrec,QT_TRANSLATE_NOOP("pigalePaint", "Polrec")},//8
    {DrawTriangle,QT_TRANSLATE_NOOP("pigalePaint", "Triangle contact")},//9
    {DrawPolar,QT_TRANSLATE_NOOP("pigalePaint","Polar")},//10
    {DrawBip2Pages,QT_TRANSLATE_NOOP("pigalePaint","2-Pages")},//11
    {0,QT_TRANSLATE_NOOP("pigalePaint","default ")}  
    };
const int border = 20;

pigalePaint::~pigalePaint()
  { }
pigalePaint::pigalePaint(QWidget *parent,pigaleWindow *f):
    QWidget(parent),mw(f),isHidden(true)
  {index = -1;
  setFocusPolicy(Qt::ClickFocus); 
  }
/*
void pigalePaint::print(QPrinter* printer)
  {if(index < 0)return;
  QPainter pp(printer);
  drawIt(&pp);
  }
*/
void pigalePaint::print(QPrinter* printer)
  {if(index < 0)return;
  QRect geo = geometry();
  resize(printer->width(),printer->width());
  QPainter pp(printer);
  drawIt(&pp);
  setGeometry(geo);
  }
void pigalePaint::image(QPrinter* printer, QString suffix)
  {if(index < 0)return;
  qApp->processEvents();
  QRect geo = geometry();
  resize(staticData::sizeImage,staticData::sizeImage);
  if(suffix == "png" || suffix == "jpg")
      {QPixmap pixmap = QPixmap::grabWidget (this); 
      pixmap.save(staticData::fileImage);
      }
  else if(suffix == "svg") 
      {QSvgGenerator *svg = new QSvgGenerator();
      svg->setFileName(staticData::fileImage);
      svg->setResolution(90); 
      svg->setSize(QSize(width(),height()));
      QPainter pp(svg);
      drawIt(&pp);
      }
  else if(suffix == "pdf" || suffix == "ps")
      {QPainter pp(printer);
      drawIt(&pp);
      }
  setGeometry(geo);
  }
void pigalePaint::drawIt(QPainter *p)
  {if(index < 0)return;
  p->fillRect(geometry(),Qt::white);
  (*DrawFunctions[index].f)(p,this);
  }
void pigalePaint::update()
  {update(index,false);}
void pigalePaint::update(int i,bool newDrawing)
  {zoom = 1;
  index = i;
  if(newDrawing) // copy the graph
      GCP = mw->GC;
  TopologicalGraph G(GCP);
  Prop1<Tpoint> pmin(G.Set(),PROP_POINT_MIN);
  Prop1<Tpoint> pmax(G.Set(),PROP_POINT_MAX);
  xmin = pmin().x();  ymin = pmin().y();
  xmax = pmax().x();  ymax = pmax().y();
  Wx_max = this->width() - 2*border;  Wy_max = this->height() - 2*border;
  xscale0 = xscale = Wx_max/(xmax - xmin);
  xtr0 = xtr  =  - xmin*xscale + border;
  yscale0 = yscale = Wy_max/(ymax - ymin);
  ytr0 = ytr  =   - ymin*yscale +border;
  mw->tabWidget->setTabText(mw->tabWidget->indexOf(this)
                            ,qApp->translate("pigalePaint",DrawFunctions[index].name)); 
  mw->tabWidget->setCurrentIndex(mw->tabWidget->indexOf(this));
  }
void pigalePaint::paintEvent(QPaintEvent * e)
  {if(isHidden)return;
  QWidget::paintEvent(e);
  QPainter p(this);
  p.setRenderHint(QPainter::Antialiasing,true);
  drawIt(&p);
  }
void pigalePaint::showEvent(QShowEvent*)
  {isHidden = false;}
void pigalePaint::hideEvent(QHideEvent*)
  {isHidden = true;}
void pigalePaint::resizeEvent(QResizeEvent* e)
  {Wx_max = this->width() - 2*border;  Wy_max = this->height() - 2*border;
  xscale0 = xscale = Wx_max/(xmax - xmin);
  xtr0 = xtr  =  - xmin*xscale + border;
  yscale0 = yscale = Wy_max/(ymax - ymin);
  ytr0 = ytr  =  - ymin*yscale + border;
  QWidget::resizeEvent(e);
  }
void pigalePaint::keyPressEvent(QKeyEvent *k)
  {int key = k->key();

  if(key == Qt::Key_Up)
      zoom = 1.1;
  else if(key == Qt::Key_Down)
      zoom = 1/1.1;
  else if(key == Qt::Key_Home)
      {xtr = xtr0; ytr = ytr0;
      xscale = xscale0; yscale = yscale0;
      }
  else
      return;
  //k->accept();
  if(key !=  Qt::Key_Home)
      {double xx0 = ((double)posClick.x() - xtr)/xscale;
      double yy0 = -((double)posClick.y() + ytr - this->height())/yscale;
      xscale *= zoom ;yscale *= zoom;
      xtr += posClick.x() - to_x(xx0);
      ytr += to_y(yy0) -  posClick.y();
      }
  repaint(geometry());
  }
void pigalePaint::wheelEvent(QWheelEvent *event)
  {//event->accept();
  zoom = (event->delta() > 0) ? 1.1 : 1./1.1;
  double xx0 = ((double)posClick.x() - xtr)/xscale;
  double yy0 = -((double)posClick.y() + ytr - this->height())/yscale;
  xscale *= zoom ;yscale *= zoom;
  xtr += posClick.x() - to_x(xx0);
  ytr += to_y(yy0) -  posClick.y();
  repaint(geometry());
  }
void pigalePaint::mousePressEvent(QMouseEvent *event)
  {posClick = event->pos();
  }
void pigalePaint::mouseMoveEvent(QMouseEvent *event)
  {int dx = event->pos().x() - posClick.x();
  int dy = event->pos().y() - posClick.y();
  posClick = event->pos();
  xtr += dx;  ytr -= dy;
  repaint(geometry());
  }
int pigalePaint::to_x(double x)
  {return (int)(x*xscale + xtr +.5);
  }
int pigalePaint::to_y(double y)
  {return (int)(this->height() - y*yscale -ytr + .5);
  }
void pigalePaint::DrawSeg(QPainter *p,Tpoint &a,Tpoint &b,int col,int width)
  {QPen pn = p->pen();
  QPoint ps = QPoint(to_x(a.x()),to_y(a.y()));
  QPoint pt = QPoint(to_x(b.x()),to_y(b.y()));
  pn.setColor(color[bound(col,1,16)]); pn.setWidth(width);
  p->setPen(pn);
  p->drawLine(ps,pt);
  }
void pigalePaint::DrawRect(QPainter *p,Tpoint &a,double nx,double ny,int col)
// draw a rectangle centered at a
  {QPen pn = p->pen();pn.setWidth(2);pn.setColor(color[Black]);p->setPen(pn);
  QBrush pb = p->brush();pb.setStyle(Qt::SolidPattern);
  pb.setColor(color[bound(col,1,16)]);p->setBrush(pb);
  //  nx *= xscale; ny *= yscale;
  //  p->drawRect(QRect(to_x(a.x()-nx/2+.5),to_y(a.y()-ny/2+.5),(int)(nx+.5),(int)(ny+.5)));
  p->drawRect(QRect((int)(to_x(a.x()) - nx*Min(xscale,yscale)/2), (int)(to_y(a.y())- ny*Min(xscale,yscale)/2), (int)(nx*Min(xscale,yscale)), (int)(ny*Min(xscale,yscale))));
  }
void pigalePaint::DrawText(QPainter *p,double x,double y,QString txt)
  {QPen pn = p->pen();pn.setWidth(1);pn.setColor(color[Black]);p->setPen(pn);
  p->drawText(to_x(x),to_y(y),txt);
  }
void pigalePaint::DrawText(QPainter *p,Tpoint &a,tvertex v,int col,int center)
// draw text centered at a, with a surrounding rectangle
// center=1 center
// center=0 horizontal
  {QString t =  getVertexLabel(GCP,v);
  QPen pn = p->pen();pn.setWidth(1);pn.setColor(color[Black]);p->setPen(pn);
  QSize size = QFontMetrics(p->font()).size(Qt::AlignCenter,t);
  double nx = size.width() + 4; double ny = size.height();
  if(t.length() == 0)nx = ny= 8;
  QRect rect;
  //if pn.setWidth() > 1 => rect increase
  if(center)
      rect = QRect((int)(to_x(a.x())-nx/2+.5),(int)(to_y(a.y())-ny/2+.5),(int)(nx+.5),(int)(ny+.5));
  else
      rect = QRect((int)(to_x(a.x())-nx/2+.5),(int)(to_y(a.y())-ny+1.),(int)(nx+.5),(int)(ny+.5));
  QBrush pb = p->brush();
  pb.setStyle(Qt::SolidPattern);
  pb.setColor(color[bound(col,0,16)]);
  //pb.setColor(color[White]);
  p->setBrush(pb);
  //pn.setColor(color[col]);pn.setWidth(1);p->setPen(pn);
  pn.setWidth(1);pn.setColor(color[Black]);p->setPen(pn);
  p->drawRect(rect);
  if(ny < 6)return;
  p->drawText(rect,Qt::AlignCenter,t);
  }
void pigalePaint::DrawText(QPainter *p,double x,double y,double nx,double ny,tvertex v,int col)
// draw centered text in rectangle left at x,y of size: nx,ny
  {QPen pn = p->pen();pn.setColor(color[Black]);
  nx *= xscale;  ny *= yscale;
  QRect rect = QRect(to_x(x),to_y(y),(int)(nx+.5),(int)(ny+.5));
  QBrush pb = p->brush();pb.setStyle(Qt::SolidPattern);
  pb.setColor(color[bound(col,1,16)]);p->setBrush(pb);
  pn.setWidth(1);p->setPen(pn);
  p->drawRect(rect);
  pn.setWidth(1);p->setPen(pn);
  QString t = getVertexLabel(GCP,v);
  p->drawText(rect,Qt::AlignCenter,t);
  }
void pigalePaint::DrawTriangle(QPainter *p,Tpoint &p1,Tpoint &p2,Tpoint &p3,int col)
  {QPen pn = p->pen();pn.setWidth(1);pn.setColor(color[Black]);p->setPen(pn);
  QBrush pb = p->brush();pb.setStyle(Qt::SolidPattern);
  pb.setColor(color[bound(col,1,16)]);p->setBrush(pb);
  QPoint vertices[3];
  vertices[0] = QPoint((int)to_x(p1.x()),(int)to_y(p1.y()));
  vertices[1] = QPoint((int)to_x(p2.x()),(int)to_y(p2.y()));
  vertices[2] = QPoint((int)to_x(p3.x()),(int)to_y(p3.y()));
  p->drawConvexPolygon(vertices,3);
  }
