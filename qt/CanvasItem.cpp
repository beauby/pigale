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
\file CanvasItem.cpp
\ingroup editor
\brief  All QGraphicsItem used by QGraphicsScene *canvas
*/

#include "pigaleWindow.h"
#include "mouse_actions.h"
#include "GraphWidget.h"
#include <TAXI/Tprop.h>
#include <QT/pigaleQcolors.h> 
#include <QT/Misc.h> 

//! The brush used by all QGraphicsItem
static QBrush *tb = 0;
//! The pen used by all QGraphicsItem
static QPen *tp = 0;

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//Methods of GraphWidget
GraphWidget::GraphWidget(QWidget *parent,pigaleWindow *_mywindow)
    : QWidget(parent)
    ,pGG(0)
  {
  NodeColorItem.resize(1,16);
  EdgeColorItem.resize(1,16);
  EdgeThickItem.resize(1,3);
  canvas = new QGraphicsScene(0,0,contentsRect().width(),contentsRect().height());
  canvas->setItemIndexMethod(QGraphicsScene::NoIndex);
  editor = new GraphEditor(this,_mywindow);
  editor->setScene(canvas);
  }
GraphWidget::~GraphWidget()
  {if(pGG)delete pGG;   
  if(canvas)delete canvas;
  }
void GraphWidget::resizeEvent(QResizeEvent*)
  {editor->setsize();
  }

//*****************************************************
// all QGraphicsItem
//*****************************************************
void CreatePenBrush()
//! creates the pen and the brush used by all QGraphicsItem
  {if(!tb) tb = new QBrush( Qt::red );
  if(!tp) tp = new QPen( Qt::black );
  }
ColorItem:: ColorItem(GraphWidget* g,QRectF &rect,int pcolor,int bcolor,bool is_node)
    :QGraphicsRectItem(rect)
//hub    :QGraphicsRectItem(rect,0,g->canvas)
    ,brush_color(bcolor),node(is_node)
  {gwp = g;
  g->canvas->addItem(this);
  tp->setColor(color[pcolor]);  tp->setWidth(3); setPen(*tp);
  tb->setColor(color[bcolor]);  setBrush(*tb); 
  setZValue(col_z);
  }
void ColorItem::SetPenColor(int pcolor)
  {tp->setColor(color[pcolor]);tp->setWidth(3);setPen(*tp);
  }
void CreateColorItems(GraphWidget* gwp,int color_node,int color_edge)
//! creates the coloured rectangles used to recolor edges and vertices
  {ColorItem *coloritem;
  int x = (int)gwp->canvas->width() - sizerect -space;
  int y = space;
  int i;
  // ColorItems for vertices
  for(i = 1;i <= 16;i++)
      {QRectF rect(x,y,sizerect,sizerect);
      if(i == color_node)
	  coloritem = new ColorItem(gwp,rect,i,i,true);
      else
	  coloritem = new ColorItem(gwp,rect,White,i,true);
      gwp->NodeColorItem[i] = coloritem;
      y = y + sizerect + space;
      }
  // ColorItems for edges
  y = (int)gwp->canvas->height() - 20*(sizerect + space);
  for(i = 1;i <= 16;i++)
      {QRectF rect(x,y,sizerect,sizerect);
      if(i == color_edge)
	  coloritem = new ColorItem(gwp,rect,i,i,false);
      else
	  coloritem = new ColorItem(gwp,rect,White,i,false);
      gwp->EdgeColorItem[i] = coloritem;
      y = y + sizerect + space;
      }
  }
ThickItem:: ThickItem(GraphWidget* g,const QRectF &rect,int ewidth,int bcolor)
    :QGraphicsRectItem(rect)
//hub    :QGraphicsRectItem(rect,0,g->canvas)
  ,brush_color(bcolor),width(ewidth)
  {gwp = g;
  g->canvas->addItem(this);
  tp->setColor(color[Black]);  tp->setWidth(ewidth); setPen(*tp);
  tb->setColor(color[bcolor]);  setBrush(*tb); 
  setZValue(thick_z);
  }
void ThickItem::SetBrushColor(int bcolor)
  {tb->setColor(color[bcolor]);  setBrush(*tb); 
  }
void CreateThickItems(GraphWidget* gwp,int width_edge)
  {ThickItem *thickitem;
  int x = (int)gwp->canvas->width() - sizerect -space;
  int y = (int)gwp->canvas->height() - 3*(sizerect + space);
  for(int i = 1;i <= 3;i++)
      {QRectF rect(x,y,sizerect,sizerecth);
      if(i == width_edge)
	  thickitem = new ThickItem(gwp,rect,i,Yellow);
      else
	  thickitem = new ThickItem(gwp,rect,i,White);
      gwp->EdgeThickItem[i] = thickitem;
      y = y + sizerect + space;
    }
  }
//*****************************************************
LineItem::LineItem(GraphWidget* g)
    :QGraphicsLineItem()
//hub    :QGraphicsLineItem(0,g->canvas)
{g->canvas->addItem(this);}
//*****************************************************
CursItem::CursItem(tvertex &_v,QPoint &p,GraphWidget* g)
    :QGraphicsLineItem()
//hub    :QGraphicsLineItem(0,g->canvas)

  {v = _v;
  g->canvas->addItem(this);
  tp->setColor(Qt::green);tp->setWidth(2);setPen(*tp);
  setLine(p.x(),p.y(),p.x(),p.y());
  setZValue(curs_z);
  }
void CursItem::setToPoint(int x,int y)
  {setLine(line().p1().x(),line().p1().y(),x,y);
  }
//*****************************************************
InfoItem::InfoItem(GraphWidget* g,QString &t,QPoint &p)
  :QGraphicsSimpleTextItem(t)
//hub  :QGraphicsSimpleTextItem(t,0,g->canvas)
  {g->canvas->addItem(this);
  setFont(QFont("sans",g->fontsize));
  tb->setColor(Qt::blue);  setBrush(*tb); 
  setPos(p.x(),p.y());
  setZValue(info_z); 
  }
InfoItem* CreateInfoItem(GraphWidget* gwp,QString &t,QPoint &p)
  {QFont font = QFont("sans",gwp->fontsize);
  QSize size = QFontMetrics(font).size(Qt::AlignCenter,t);
  int dx =size.width() + 8;  int dy =size.height() +4;
  p.ry() -= dy;
  p.rx() = bound(p.x(),dx/2,gwp->canvas->width()-dx/2);
  p.ry() = bound(p.y(),dy/2,gwp->canvas->height()-dy/2);
  QRectF rect = QRectF(p.x()-dx/2,p.y()-dy/2,dx,dy);
  QGraphicsRectItem *rectitem = new QGraphicsRectItem(rect);
  gwp->canvas->addItem(rectitem);
//hub  QGraphicsRectItem *rectitem = new QGraphicsRectItem(rect,0,gwp->canvas);
  QPoint q  = QPoint(p.x()-dx/2+2,p.y()-size.height()/2);
  InfoItem *infoitem  = new InfoItem(gwp,t,q);
  tp->setWidth(2); tp->setColor(Qt::red); tb->setColor(Qt::white);
  rectitem->setBrush(*tb);rectitem->setPen(*tp);
  rectitem->setZValue(inforect_z);
  infoitem->rectitem = rectitem;
  return infoitem;
  }
//*****************************************************
ArrowItem::ArrowItem(EdgeItem *edgeitem)
    :QGraphicsPolygonItem(edgeitem)
  //hub  :QGraphicsPolygonItem(0,edgeitem->scene())
  {edgeItem= edgeitem;
  pts.resize(4);
  ComputeCoord();
  setZValue(arrow_z);
  setFlags(0);
  }
void ArrowItem::ComputeCoord()
  {prepareGeometryChange();
  QPointF u =  edgeItem->line().p2() - edgeItem->line().p1();
  double ml = sqrt(double(u.x()*u.x() + u.y()*u.y()))+1.5;
  double diviseur = 12;
  // for short edges or long edges
  if(ml > 10 && ml < 50){diviseur = (diviseur*ml)/50;}
  else if(ml > 100) {diviseur = (diviseur*ml)/100;}
  diviseur = Max(diviseur,1);
  QPointF v = QPointF(-u.y()/diviseur,u.x()/diviseur);
  QPointF p0 =  edgeItem->line().p2();
  pts[0] = p0;
  QPointF p1 =  p0 - v -(u*2)/diviseur;
  pts[1] = p1;
  pts[2] = p0  -u/diviseur;
  QPointF p3 = p0 + v - (u*2)/diviseur;
  pts[3] = p3;
  setPolygon(pts);
  }
void ArrowItem::SetColor(QColor col)
  {tp->setColor(col);  tb->setColor(col); 
  setPen(*tp);setBrush(*tb);
  }
//*****************************************************
EdgeItem* CreateEdgeItem(tedge &e,GraphWidget* g)
  {GeometricGraph & G = *(g->pGG);
  Prop<bool> eoriented(G.Set(tedge()),PROP_ORIENTED,false);
  Prop<NodeItem *> nodeitem(G.Set(tvertex()),PROP_CANVAS_ITEM);
  tvertex v0 = G.vin[e];  tvertex v1 = G.vin[-e];
  double h = g->canvas->height();
  double x0 = G.vcoord[v0].x();  double y0 = G.vcoord[v0].y();
  double x1 = G.vcoord[v1].x();  double y1 = G.vcoord[v1].y();
  double x  = x0 * xorient + x1*(1.-xorient);
  double y  = y0 * xorient + y1*(1.-xorient);
  QColor col = color[bound(G.ecolor[e],1,16)];
  QColor col2 = col;
  if (G.Set(tedge()).exist(PROP_COLOR2))
      {Prop<short> ecolor2(G.Set(tedge()),PROP_COLOR2);
      ecolor2.definit(1);
      col2 = color[bound(ecolor2[e],1,16)];
      }
  // first EdgeItem representing the lower part
  tp->setColor(col);
  tp->setWidth(G.ewidth[e]);
  EdgeItem *edge0 = new EdgeItem(g,e,x0,h-y0,x,h-y,true,nodeitem(G.vin[e]));
  edge0->arrow->SetColor(col);
  // second EdgeItem  representing the upper part
  if(staticData::ShowOrientation() && eoriented[e])
      {edge0->arrow->show();
      tp->setColor(Desaturate(col));
      col = Desaturate(col);
      }
  else
      {edge0->arrow->hide();
      tp->setColor(col2);
      col = col2;
      }
  EdgeItem *edge1 = new EdgeItem(g,e,x,h-y,x1,h-y1,false,nodeitem(G.vin[-e]));
  edge0->opp = edge1;  edge1->opp = edge0;
  return edge0;
  }
EdgeItem::EdgeItem(GraphWidget* g,tedge &ee,double x_from,double y_from,double x_to,double y_to
                   ,bool _lower,NodeItem *_node)
    :QGraphicsLineItem()
//hub    :QGraphicsLineItem(0,g->canvas)
  {gwp = g;
  g->canvas->addItem(this);
  lower = _lower;
  e = ee;
  node = _node;
  setLine(x_from,y_from,x_to,y_to);
  if(lower)
      arrow = new  ArrowItem(this);
  else 
      arrow = NULL;
  setPen(*tp);
  setZValue(edge_z);
  setFlags(0);
  }
void EdgeItem::paint(QPainter *painter,const QStyleOptionGraphicsItem *,QWidget *)
  {painter->setPen(pen());
  painter->drawLine(line());
  } 
QRectF EdgeItem::boundingRect() const
  {return QRectF(line().p1(),line().p2());  
  }
void EdgeItem::SetColor(QColor c,bool both)
  {GeometricGraph & G = *(gwp->pGG);
  Prop<bool> eoriented(G.Set(tedge()),PROP_ORIENTED,false);
  tp->setColor(c);tp->setWidth(G.ewidth[e]);
  setPen(*tp);
  if(lower && both)
      {if(eoriented[this->e] && staticData::ShowOrientation())
	  opp->SetColor(Desaturate(c));
      else
	  opp->SetColor(c);
      arrow->SetColor(c);
      }
  }
void EdgeItem::SetColors(QColor c1, QColor c2)
  {GeometricGraph & G = *(gwp->pGG);
  Prop<bool> eoriented(G.Set(tedge()),PROP_ORIENTED,false);
  tp->setColor(c1);tp->setWidth(G.ewidth[e]);
  setPen(*tp);
  if(lower)
      opp->SetColor(c2);
  else
      opp->SetColor(c1);
  }
void EdgeItem::setFromPoint(double x,double y)
  {prepareGeometryChange(); // ralenti mais utile ???
  setLine(x,y,line().p2().x(),line().p2().y());
  }
void EdgeItem::setToPoint(double x,double y)
  {prepareGeometryChange();
  setLine(line().p1().x(),line().p1().y(),x,y);
  }
//*****************************************************
NodeItem::NodeItem(tvertex &_v,GraphWidget* g,QRectF &rect,QColor &col,QString &_t)
    :QGraphicsRectItem(rect)
//hub    :QGraphicsRectItem(rect,0,g->canvas)
  {gwp = g;
  g->canvas->addItem(this);
  v = _v;
  t = _t;
  vcolor = col;
  tcolor = OppCol(col);
  setZValue(node_z);
  setFlags(0);
  }
QRectF NodeItem::boundingRect() const
  {return QRectF(rect());  
  }
void NodeItem::paint(QPainter *painter,const QStyleOptionGraphicsItem * option,QWidget *)
  {tp->setColor(Black);
  tp->setWidth(1);
  painter->setPen(*tp);
  tb->setColor(vcolor);
  painter->setBrush(*tb);
  painter->drawRect(option->exposedRect);
  tp->setColor(tcolor);
  painter->setPen(*tp);
  // if smartViewportUpdate:
  painter->drawText(rect(),Qt::AlignCenter,t);// so the text is completely redrawn
  //painter->drawText(option->exposedRect,Qt::AlignCenter,t);
  }
NodeItem* CreateNodeItem(tvertex &v,GraphWidget* gwp)
  {GeometricGraph & G = *(gwp->pGG);
  double x =  G.vcoord[v].x();
  double y =  gwp->canvas->height() - G.vcoord[v].y();
  QString t = getVertexLabel(G.Container(),v);
  QFont font = QFont("sans",gwp->fontsize);
  QSize size = QFontMetrics(font).size(Qt::AlignCenter,t);
  int dx = size.width() + 4;  int dy = size.height();
  if(t.length() == 0){dx = 8; dy = 8;}
  QRectF rect = QRectF(x-dx/2,y-dy/2,dx,dy);
  QColor col = color[G.vcolor[v]];
  return  new NodeItem(v,gwp,rect,col,t);
  }
void NodeItem::SetText(QString _t)
  {t = _t;
  QFont font = QFont("sans",gwp->fontsize);
  QSize size = QFontMetrics(font).size(Qt::AlignCenter,t);
  int dx =size.width() + 4;  int dy =size.height();
  if(t.length() == 0){dx = 8; dy = 8;}
  prepareGeometryChange();
  setRect(QRectF(rect().center().x()-dx/2,rect().center().y()-dy/2,dx,dy));
  }
void NodeItem::SetColor(QColor c)
  {vcolor = c;
  update();
  }
void NodeItem::moveTo(Tpoint &p,double eps)
//! /fn  does not modify vertex coordinates: only used in spring embedders
  {QPointF qp = QRectF(rect()).center() + pos(); //position initiale + deplacement
  double dx = p.x() - qp.x();
  double dy = gwp->canvas->height() - p.y() - qp.y();
  if(Abs(dx) < eps && Abs(dy) < eps)return;
  GeometricGraph & G = *(gwp->pGG);
  Prop<EdgeItem *> edgeitem(G.Set(tedge()),PROP_CANVAS_ITEM);
  double nx = p.x(); //new x position
  double ny = gwp->canvas->height() - p.y();
  QGraphicsRectItem::moveBy(dx,dy);
  // Move incident edges
  EdgeItem *ei,*up;
  double x0,y0;
  double x,y;
  for(tbrin b=G.FirstBrin(v);b!=0;b=G.NextBrin(v,b))
      {ei = (EdgeItem *)(edgeitem[b.GetEdge()]); //lower part
      up = ei->opp; //upper part
      if(b() > 0)  
	  {x0 = up->line().p2().x(); y0 = up->line().p2().y();
	  x  = nx * xorient + x0*(1.-xorient);
	  y  = ny * xorient + y0*(1.-xorient);
	  ei->setLine(nx,ny,x,y); 
 	  up->setFromPoint(x,y);
	  }
      else  
	  {x0 = ei->line().p1().x(); y0 = ei->line().p1().y();
	  x  = x0 * xorient + nx*(1.-xorient);
	  y  = y0 * xorient + ny*(1.-xorient);
	  up->setLine(x,y,nx,ny);
 	  ei->setToPoint(x,y);
	  }
      ei->arrow->ComputeCoord();
      }
  }
 void NodeItem::moveBy(double dx, double dy)
//! /fn  modify vertex coordinates
  {QGraphicsRectItem::moveBy(dx,dy);
  GeometricGraph & G = *(gwp->pGG);
  G.vcoord[v].x() += dx;   G.vcoord[v].y() -= dy; 
  // Move incident edges
  double nx = G.vcoord[v].x();  
  double ny = gwp->canvas->height() - G.vcoord[v].y();
  Prop<EdgeItem *> edgeitem(G.Set(tedge()),PROP_CANVAS_ITEM);
  EdgeItem *ei,*up;
  double x0,y0;
  double x,y;
  for(tbrin b=G.FirstBrin(v);b!=0;b=G.NextBrin(v,b))
      {ei = (EdgeItem *)(edgeitem[b.GetEdge()]); //lower part
      up = ei->opp; //upper part
      if(b() > 0)  
	  {x0 = up->line().p2().x(); y0 = up->line().p2().y();
	  x  = nx * xorient + x0*(1.-xorient);
	  y  = ny * xorient + y0*(1.-xorient);
          ei->setLine(nx,ny,x,y);
	  up->setFromPoint(x,y);//updates the incident node
	  }
      else  
	  {x0 = ei->line().p1().x(); y0 = ei->line().p1().y();
	  x  = x0 * xorient + nx*(1.-xorient);
	  y  = y0 * xorient + ny*(1.-xorient);
	  ei->setToPoint(x,y);    
          up->setLine(x,y,nx,ny);//updates the incident node
	  }
      ei->arrow->ComputeCoord();
      }
  }
//*****************************************************
void GraphEditor::DrawGrid(const Tgrid &grid)
/*! input: min_used_x, max_used_x, nxstep (id for y)
  compute xstep and the grid
*/
  {if(GridDrawn)clearGrid();
  if( grid.delta.x() < 7 ||  grid.delta.y() < 7){clearGrid();return;}
  QGraphicsLineItem *line;
  tp->setColor(color[Grey2]);tp->setWidth(1);
  double x0 = grid.orig.x() - (int)(grid.orig.x()/grid.delta.x())*grid.delta.x();
  double dy = gwp->canvas->height()- grid.orig.y();
  double y0 = dy - (int)(dy/grid.delta.y())*grid.delta.y();
  double x1 = gwp->canvas->width() -space -sizerect;
  x1 = (int)((x1-x0)/grid.delta.x())*grid.delta.x() + x0 +1.5;
  double y1 = gwp->canvas->height();
  y1 =  (int)((y1-y0)/grid.delta.y())*grid.delta.y() + y0 +1.5;
  double x,y;
  double i = .0;
  // we move the grid by one pixel
  x0 += .5; y0 += .5;
  for(;;) //horizontales
      {y = i++ * grid.delta.y() + y0 +.5;
      if(y > y1 )break;
      line = new LineItem(gwp);
      line->setLine((int)x0,(int)y,(int)x1,(int)y);
      line->setPen(*tp); line->setZValue(grid_z); 
      if(ShowGrid) line->show();
      else line->hide();
      }
  i = .0;
  for(;;) //verticales
      {x =i++*grid.delta.x() + x0 + .5;
      if(x > x1)break;
      line = new LineItem(gwp);
      line->setLine((int)x,(int)y0,(int)x,(int)y1);
      line->setPen(*tp); line->setZValue(grid_z);
      if(ShowGrid) line->show();
      else line->hide();
      }
    GridDrawn = true;
  }
