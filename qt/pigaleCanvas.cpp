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
\file pigaleCanvas.cpp
\ingroup editor
\brief  Constructor, mouse events,...
 */

#include "pigaleWindow.h"
#include "GraphWidget.h"
#include "mouse_actions.h"
#include "gprop.h"
#include <TAXI/Tbase.h> 
#define COLORNAMES
#include <QT/pigaleQcolors.h> 
#include <QT/Misc.h> 

#include <QInputDialog>
#include <QSvgGenerator>
#include <QGLWidget>

GraphEditor::GraphEditor(GraphWidget* parent,pigaleWindow *_mywindow)
    :QGraphicsView(parent->canvas,parent)
    ,gwp(parent),mywindow(_mywindow)
    ,DoNormalise(true),is_init(false)
    ,color_node(Yellow),color_edge(Black),width_edge(1)
    ,zoom(1.),ShowGrid(false),FitToGrid(false),SizeGrid(100)
  {CreatePenBrush();
  setFocusPolicy(Qt::ClickFocus);
  setRenderHints(QPainter::Antialiasing);
  //much faster: but no antialiasing if no Samplebuffers
  //setViewport(new QGLWidget);
  setViewportUpdateMode(FullViewportUpdate);
  }
void GraphEditor::update(int compute)
// called when loading a graph compute = 1 or -1
// if we only have erased edges and/or vertice compute = 0
  {if(gwp->pGG)delete gwp->pGG;
  gwp->pGG = new GeometricGraph(mywindow->GC);
  gwp->moving_item = 0;  gwp->curs_item = 0;  gwp->info_item = 0; gwp->moving_subgraph = false;
  if(compute)
      {mywindow->mouse_actions->ButtonFitGrid->setChecked(false);
      mywindow->mouse_actions->ButtonUndoGrid->setDisabled(true);
      SizeGrid = FitSizeGrid = OldFitSizeGrid = 100;
      mywindow->mouse_actions->LCDNumberX->display(100);
      mywindow->mouse_actions->LCDNumberY->display(100);
      nxstep = nystep  = SizeGrid;
      GridDrawn = false;
      ForceGrid  =false;
      RedrawGrid =true;
      zoom = 1.;
      load(true);
      }
  else
      load(false);
  if(compute != -1)mywindow->tabWidget->setCurrentIndex(mywindow->tabWidget->indexOf(gwp)); 
  }
void GraphEditor::keyPressEvent(QKeyEvent *k)
  {key_pressed = k->key();
  if(key_pressed  == Qt::Key_Up)Zoom(1);
  else if(key_pressed  == Qt::Key_Down)Zoom(-1);
  else if(key_pressed  == Qt::Key_Home)Zoom(0);
  else k->ignore();
  }
void GraphEditor::wheelEvent(QWheelEvent *event)
  {if(event->modifiers() ==  Qt::ShiftModifier)return;
  int dir = (event->delta() > 0) ? 1 : -1;
  Zoom(dir);
  }
void GraphEditor::mousePressEvent(QMouseEvent* e)
  {GeometricGraph & G = *(gwp->pGG);
  QPoint p((int)e->pos().x(),(int)e->pos().y());
  setFocus(); // as we might loose the focus
  ColorItem *coloritem;
  if(FindItem(p,coloritem) != 0)
      {if(e->button() == Qt::LeftButton)
          {if(coloritem->node)
              {gwp->NodeColorItem[color_node]->SetPenColor(White);
              coloritem->SetPenColor(coloritem->brush_color);
              color_node = coloritem->brush_color;
              G.vcolor.definit(color_node);
              }
          else
              {gwp->EdgeColorItem[color_edge]->SetPenColor(White);
              coloritem->SetPenColor(coloritem->brush_color);
              color_edge = coloritem->brush_color;
              G.ecolor.definit(color_edge);
              }
          return;
          }
      else if(e->button() == Qt::RightButton)
          {QString t;
          if(coloritem->node)
              t.sprintf("Vertex color:%s",ColorName[coloritem->brush_color]);
          else
              t.sprintf("Edge color:%s",ColorName[coloritem->brush_color]);
          gwp->info_item = CreateInfoItem(gwp,t,p);
          setCursor(QCursor(Qt::WhatsThisCursor));
          return;
          }
      }
  ThickItem *thickitem;
  if(FindItem(p,thickitem) != 0)
      {if(e->button() == Qt::LeftButton)
          {gwp->EdgeThickItem[width_edge]->SetBrushColor(White);
          thickitem->SetBrushColor(Yellow);
          width_edge = thickitem->width;
          G.ewidth.definit(width_edge);
          return;
          }
      else if(e->button() == Qt::RightButton)
          {QString t;
          t.sprintf("Edge width:%d",thickitem->width);
          gwp->info_item = CreateInfoItem(gwp,t,p);
          setCursor(QCursor(Qt::WhatsThisCursor));
          return;
          }
      }
  if(e->button() == Qt::RightButton )
      {NodeItem* node;
      EdgeItem *edge;
      QString t;
      int rtt = FindItem(p,node,edge);
      if(rtt == 0)rtt = FindItem(p,edge);//augment the collision zone
      if(rtt == node_rtti)
          if (G.Set().exist(PROP_VSLABEL) && G.Set(tvertex()).exist(PROP_SLABEL))
              {QString tt;
              Prop1<svector<tstring *> >vslabel(G.Set(),PROP_VSLABEL);
              Prop<int> slabel(G.Set(tvertex()),PROP_SLABEL,0);
              if (vslabel()[slabel[node->v]]==(tstring *)0) tt="(null)";
              else tt=~(*(vslabel()[slabel[node->v]]));
              t.sprintf("vertex:%d Label:%d (%s)",(node->v)(),G.vlabel[node->v],(const char *)tt.toLatin1());
              }
          else
              t.sprintf("vertex:%d Label:%d",(node->v)(),G.vlabel[node->v]);
      else if(rtt == edge_rtti)
          {double d = Distance(G.vcoord[G.vin[edge->e]],G.vcoord[G.vin[-(edge->e)]])+.5;
          t.sprintf("edge:%d Label:%d len:%d",(edge->e)(),G.elabel[edge->e],(int)d);
          }
      else
          {t.sprintf("(%d,%d)",(int)e->pos().x(),(int)e->pos().y());
          setCursor(QCursor(Qt::BlankCursor));
          }
      gwp->info_item = CreateInfoItem(gwp,t,p);
      return;
      }
  //GetMouseAction_1 returns the active radio button (0,5)
  int MouseAction = GetMouseAction_1();
  bool Shift    =  e->modifiers() == Qt::ShiftModifier;
  bool Control  =  e->modifiers() ==  Qt::ControlModifier;
  bool SControl =  e->modifiers() ==  (Qt::ShiftModifier | Qt::ControlModifier);

  if(e->button() == Qt::MidButton) //move
      MouseAction = (Shift) ? -3 : 3;
  else if(Shift && MouseAction == 1)        //add
      MouseAction = -1;                     // delete vertex or edge
  else if(Control && MouseAction == 1)
      MouseAction = 10;                     //duplicate graph
  else if(SControl && MouseAction == 1)
      MouseAction = 11;                     //duplicate +
  else if(Shift && MouseAction == 2)        //Orient or reorient if oriented
      MouseAction = -2;                     //disorient
  else if(Shift && MouseAction == 3)        //move vertices
      MouseAction = -3;                     //move colored vertices
  else if(Shift && MouseAction == 4)        //bissect
      MouseAction = -4;                     //contract
  else if(Shift && MouseAction == 5)        //define exterior face
      MouseAction = -5;                     //define extbrin
  else if(Shift && MouseAction == 6)        //define a label
      MouseAction = -6;                     //reset all labels to indices
  
  if(MouseAction == 0) // color
      {NodeItem* node;
      EdgeItem *edge;
      int rtt = FindItem(p,node,edge);
      if(rtt == 0) //augment the collision zone
          {rtt = FindItem(p,edge);
          if(rtt == 0)return;
          } 
      if(rtt == node_rtti)
          {mywindow->UndoTouch(false);
          G.vcolor[node->v] = color_node;
          node->SetColor(color[color_node]);
          }
      else if(rtt == edge_rtti)
          {mywindow->UndoTouch(false);
          G.ecolor[edge->e] = color_edge;
          G.ewidth[edge->e]  = width_edge;
          if(edge->lower) edge->SetColor(color[color_edge]);
          else edge->opp->SetColor(color[color_edge]);
          }
      return; 
      }
  else if(MouseAction == 1) // Start create an edge
      {mywindow->UndoTouch(false);
      Prop<NodeItem *> nodeitem(G.Set(tvertex()),PROP_CANVAS_ITEM);
      NodeItem* node;
      EdgeItem *edge;
      tvertex v;
      int h = (int)gwp->canvas->height();
      int rtt = FindItem(p,node,edge);
      if(rtt != node_rtti)// add a vertex
          {Tpoint pp((double)p.x(),(double)(h - p.y()));
          v = G.NewVertex(pp);
          ToGrid(v);
          nodeitem[v] = node  = CreateNodeItem(v,gwp);
          mywindow->mouse_actions->ButtonUndoGrid->setDisabled(true);
          }
      else
          v = node->v;
      start_position = QPoint((int)G.vcoord[v].x(),h-(int)G.vcoord[v].y());
      node->SetColor(Qt::red);
      gwp->curs_item = new CursItem(v,start_position,gwp);
      }
  else if(MouseAction == -1) // Delete
      {NodeItem* node;
      EdgeItem *edge;
      int rtt = FindItem(p,node,edge);
      if(rtt == 0) //augment the collision zone
          {rtt = FindItem(p,edge);
          if(rtt == 0)return;
          } 
      if(rtt == node_rtti)
          {mywindow->UndoTouch(true);
          G.DeleteVertex(node->v);
          mywindow->mouse_actions->ButtonUndoGrid->setDisabled(true);
          }
      else
          {mywindow->UndoTouch(true);
          G.DeleteEdge(edge->e);
          }
      load(false);
      mywindow->information();// Informations
      PrintSizeGrid();
      return;
      }
  else if(MouseAction == 3) // Start moving a vertex
      {NodeItem* node;
      EdgeItem *edge;
      int rtt = FindItem(p,node,edge);
      if(rtt != node_rtti)return;
      mywindow->UndoTouch(false);
      gwp->moving_item = node;
      start_position = p;
      return;
      }
  else if(MouseAction == -3) // Start moving a subgraph
      {mywindow->UndoTouch(false);
      gwp->moving_subgraph = true;
      start_position = p;
      return;
      }
  else if(MouseAction == 4) // Bissect an edge
      {mywindow->UndoTouch(true);
      if(FitToGrid)//and we are sure that ButtonFitGrid exists
	  mywindow->mouse_actions->ButtonFitGrid->setChecked(false);
      mywindow->mouse_actions->ButtonUndoGrid->setDisabled(true);
      EdgeItem *edge;
      int rtt = FindItem(p,edge);
      if(rtt != edge_rtti)return;
      G.BissectEdge(edge->e);
      DoNormalise = false;
      load(true);
      DoNormalise = true;
      mywindow->information();// Informations
      return;
      }
  else if(MouseAction == -4) // Contract an edge
      {EdgeItem *edge;
      int rtt = FindItem(p,edge);
      if(rtt != edge_rtti)return;
      mywindow->UndoTouch(true);
      // provisoire
      if(edge->lower)G.ReverseEdge(edge->e);
      // end provisoire
      G.ContractEdge(edge->e);
      load(false);
      mywindow->information();// Informations
      return;
      }
  else if(MouseAction == 5) // Color Exterior face
      {GeometricGraph & G = *(gwp->pGG);
      Prop<EdgeItem *> edgeitem(G.Set(tedge()),PROP_CANVAS_ITEM);
      Tpoint pp((double)p.x(),(double)(gwp->canvas->height()-p.y()));
      if(G.FindExteriorFace(pp) == 0)return;
      //ColorExteriorface
      tedge e;
      ForAllEdges(e,G)edgeitem[e]->SetColor(color[color_edge]);
      tbrin b0 = G.extbrin();
      tbrin b = b0;
      do
          {e = b.GetEdge();
          edgeitem[e]->SetColor(color[Red]);
          } while((b = G.cir[-b]) != b0);
      if(b0() > 0)
          edgeitem[b0.GetEdge()]->SetColor(color[Green],false);
      else
          edgeitem[b0.GetEdge()]->opp->SetColor(color[Green],false);
      return; 	  
      }
  else if(MouseAction == -5) // Define Extbrin
      {EdgeItem *edge;
      int rtt = FindItem(p,edge);
      if(rtt != edge_rtti)return;
      Prop<EdgeItem *> edgeitem(G.Set(tedge()),PROP_CANVAS_ITEM);
      tedge je;
      ForAllEdges(je,G)edgeitem[je]->SetColor(color[color_edge]);
      GeometricGraph & G = *(gwp->pGG);
      je = edge->e;
      //       Prop<NodeItem *> nodeitem(G.Set(tvertex()),PROP_CANVAS_ITEM);
      //       NodeItem *n1 =  nodeitem[G.vin[je]];   Tpoint p1(n1->x(),n1->y());
      //       NodeItem *n2 =  nodeitem[G.vin[-je]];  Tpoint p2(n2->x(),n2->y());
      //       Tpoint pp((double)p.x(),(double)p.y());
      //       G.extbrin() = (Distance2(pp,p1) < Distance2(pp,p2)) ? je.firsttbrin() : je.secondtbrin();
      G.extbrin() = (edge->lower ) ? je.firsttbrin() : je.secondtbrin();
      edge->SetColor(color[Green],false);
      return;
      }
  else if(MouseAction == 6) // Modify label
      {NodeItem* node;
      EdgeItem *edge;
      int rtt = FindItem(p,node,edge);
      if(rtt != node_rtti)return;
      tvertex v = node->v;
      bool ok;
      //int res = QInputDialog::getInteger(this,"Pigale","Enter a number:",(int)G.vlabel[node->v],0,2147483647,1,&ok);
      int res = QInputDialog::getInt(this,"Pigale","Enter a number:",(int)G.vlabel[node->v],0,2147483647,1,&ok);
      if(!ok)return;
      G.vlabel[node->v] = res;
      QString t = getVertexLabel(G.Container(),v);
      node->SetText(t);
      return;
      }
  else if(MouseAction == -6) // Reset all labels
      {Prop<NodeItem *> nodeitem(G.Set(tvertex()),PROP_CANVAS_ITEM);
      for(tvertex v = 1; v <= G.nv();v++)
          {G.vlabel[v] = v();
          nodeitem[v]->SetText(QString("%1").arg(v()));
          }
      for(tedge e = 1; e <= G.ne();e++)
          G.elabel[e] = e();
      }
  else if(MouseAction == 2  || MouseAction == -2) // Orient/Reverse or deorient
      {Prop<bool> eoriented(G.Set(tedge()),PROP_ORIENTED,false);
      Prop<bool> reoriented(G.Set(tedge()),PROP_REORIENTED); 
      Prop<EdgeItem *> edgeitem(G.Set(tedge()),PROP_CANVAS_ITEM);
      mywindow->setShowOrientation(true);
      EdgeItem *edge;
      int rtt = FindItem(p,edge);
      if(rtt != edge_rtti)return;
      mywindow->UndoTouch(true);
      if(MouseAction == 2)
          {if(eoriented[edge->e])
              {G.ReverseEdge(edge->e); eoriented[edge->e] = true;
              reoriented[edge->e] = false;
              edgeitem[edge->e] = CreateEdgeItem(edge->e,gwp); 
	      if(edge->arrow)
                  scene()->removeItem(edge->arrow);
	      else 
                  scene()->removeItem(edge->opp->arrow);
	      scene()->removeItem(edge->opp);scene()->removeItem(edge);
              mywindow->information();// Informations
              return;
              }
          else
              {eoriented[edge->e] = true;
	      if(edge->lower)
                  {G.ReverseEdge(edge->e);reoriented[edge->e] = false;}
	      edgeitem[edge->e] = CreateEdgeItem(edge->e,gwp);
	      if(edge->arrow)
                  scene()->removeItem(edge->arrow);
	      else 
                  scene()->removeItem(edge->opp->arrow);
	      scene()->removeItem(edge->opp);scene()->removeItem(edge);
              mywindow->information();// Informations
              return;
              }
          }
      else
          {eoriented[edge->e] = false;
	  edgeitem[edge->e] = CreateEdgeItem(edge->e,gwp); 
          if(edge->arrow)
              scene()->removeItem(edge->arrow);
          else 
              scene()->removeItem(edge->opp->arrow);
	  scene()->removeItem(edge->opp);scene()->removeItem(edge);
          mywindow->information();// Informations
          return;
          }
      }
  else if(MouseAction == 10)// Duplicate the sugraph of the current color
      {mywindow->UndoTouch(true);
      if(FitToGrid)//and we are sure that ButtonFitGrid exists
          mywindow->mouse_actions->ButtonFitGrid->setChecked(false);
      mywindow->mouse_actions->ButtonUndoGrid->setDisabled(true);
      short vcol=0;  G.vcolor.getinit(vcol);
      G.vcolor.definit((short)((vcol+1)%16));
      GeometricGraph & G = *(gwp->pGG);
      Tpoint translate(this->width()/2.,0);
      int n = G.nv();
      svector<tvertex> newvertex(1,n);
      tvertex v;
      for(v = 1; v <= n;v++)//translate all the graph
          G.vcoord[v] /= 2.;
      for(v = 1; v <= n;v++)
          {if(G.vcolor[v] != vcol)continue;
          newvertex[v] = G.NewVertex(G.vcoord[v] + translate);
          }
      int m = G.ne();
      tvertex v1,v2;
      for(tedge e = 1; e <= m;e++)
          {v1 = G.vin[e];v2 = G.vin[-e];
          if(G.vcolor[v1] == vcol && G.vcolor[v2] == vcol)
              G.NewEdge(newvertex[v1],newvertex[v2]);
          }
      load(true);
      mywindow->information();// Informations
      }
  else if(MouseAction == 11)
      //Duplicate the sugraph of the current color
      // and add edges between a new vertex and its father
      {mywindow->UndoTouch(true);
      if(FitToGrid)//and we are sure that ButtonFitGrid exists
          mywindow->mouse_actions->ButtonFitGrid->setChecked(false);
      mywindow->mouse_actions->ButtonUndoGrid->setDisabled(true);
      mywindow->mouse_actions->ButtonFitGrid->setChecked(false);
      short vcol=0;  G.vcolor.getinit(vcol);
      G.vcolor.definit((short)((vcol+1)%16));
      GeometricGraph & G = *(gwp->pGG);
      Tpoint translate(this->width()/2.,0);
      int n = G.nv();
      svector<tvertex> newvertex(1,n);
      tvertex v;
      for(v = 1; v <= n;v++)//translate all the graph
          G.vcoord[v] /= 2.;
      for(v = 1; v <= n;v++)
          {if(G.vcolor[v] != vcol)continue;
          newvertex[v] = G.NewVertex(G.vcoord[v] + translate);
          }
      int m = G.ne();
      tvertex v1,v2;
      for(tedge e = 1; e <= m;e++)
          {v1 = G.vin[e];v2 = G.vin[-e];
          if(G.vcolor[v1] == vcol && G.vcolor[v2] == vcol)
              G.NewEdge(newvertex[v1],newvertex[v2]);
          }
      for(v = 1; v <= n;v++)
          {if(G.vcolor[v] != vcol)continue;
          G.NewEdge(v,newvertex[v]);
          }
      load(true);
      mywindow->information();// Informations
      }
  }
void GraphEditor::mouseMoveEvent(QMouseEvent* e)
  {if(gwp->moving_item)
      {//setRenderHints(!QPainter::Antialiasing);
      gwp->moving_item->moveBy(e->pos().x() - start_position.x(), e->pos().y() - start_position.y());
      start_position = e->pos();
      }
  else if(gwp->moving_subgraph)
      {setRenderHints(0);
      GeometricGraph & G = *(gwp->pGG);
      Prop<NodeItem *> nodeitem(G.Set(tvertex()),PROP_CANVAS_ITEM);
      short vcol=0;  G.vcolor.getinit(vcol);
      NodeItem * node;
      for(tvertex v = 1; v <= G.nv();v++)
          {node =(NodeItem *)nodeitem[v];
          if(G.vcolor[node->v] != vcol)continue;
          node->moveBy(e->pos().x() - start_position.x(), e->pos().y() - start_position.y());
          }
      start_position = e->pos();
      }
  else if(gwp->curs_item) // moving the elastic cursor
      gwp->curs_item->setToPoint(e->pos().x(),e->pos().y());
  }
void GraphEditor::mouseReleaseEvent(QMouseEvent* event)
  {//setRenderHints(QPainter::Antialiasing);
  if(gwp->info_item) //end info
      {scene()->removeItem(gwp->info_item->rectitem);
      scene()->removeItem(gwp->info_item);
      gwp->info_item = 0;
      setCursor(QCursor(Qt::ArrowCursor));
      return;
      }
  if(gwp->moving_item) //end moving a vertex
      {if(!FitToGrid)
          {gwp->moving_item = 0;return;}
      GeometricGraph & G = *(gwp->pGG);
      NodeItem *node = gwp->moving_item;
      tvertex v = node->v;
      double prev_x = G.vcoord[v].x();
      double prev_y = G.vcoord[v].y();
      ToGrid(v);
      double dx =  G.vcoord[v].x() - prev_x;
      double dy =  G.vcoord[v].y() - prev_y;
      G.vcoord[v].x() = prev_x;
      G.vcoord[v].y() = prev_y;
      node->moveBy(dx,-dy);
      gwp->moving_item = 0;
      UpdateSizeGrid();
      PrintSizeGrid();
      return;
      }
  if(gwp->moving_subgraph == true)
      {gwp->moving_subgraph = false;
      if(!FitToGrid)return;
      GeometricGraph & G = *(gwp->pGG);
      Prop<NodeItem *> nodeitem(G.Set(tvertex()),PROP_CANVAS_ITEM);
      short vcol=0;  G.vcolor.getinit(vcol);
      NodeItem * node;
      // Find a vertex of the subgraph
      tvertex mv = 0;
      tvertex v;
      for(v = 1; v <= G.nv();v++)
          {node =(NodeItem *)nodeitem[v];
          if(G.vcolor[node->v] != vcol)continue;
          else {mv = v;break;}
          }
      if(!mv)return;
      double prev_x = G.vcoord[mv].x();
      double prev_y = G.vcoord[mv].y();
      ToGrid(mv);
      double dx =  G.vcoord[mv].x() - prev_x;
      double dy =  G.vcoord[mv].y() - prev_y;
      G.vcoord[mv].x() = prev_x;
      G.vcoord[mv].y() = prev_y;
      for(v = 1;v <= G.nv();v++)
          {node =(NodeItem *)nodeitem[v];
          if(G.vcolor[node->v] != vcol)continue;
          node->moveBy(dx,-dy);
          }
      UpdateSizeGrid();
      PrintSizeGrid();
      return;
      }
  if(gwp->curs_item)// end creating an edge
      {GeometricGraph & G = *(gwp->pGG);
      Prop<EdgeItem *> edgeitem(G.Set(tedge()),PROP_CANVAS_ITEM);
      Prop<NodeItem *> nodeitem(G.Set(tvertex()),PROP_CANVAS_ITEM);
      NodeItem* node;
      EdgeItem *edge;
      tvertex v1,v2;
      // Reset the color of starting vertex
      v1 = gwp->curs_item->v;
      node = (NodeItem *)(nodeitem[v1]); node->SetColor(color[G.vcolor[v1]]);
      QPoint p(event->pos()); 
      ToGrid(p);
      int rtt = FindItem(p,node,edge);
      v2 = (rtt != node_rtti) ? 0 : node->v;
      if(rtt != node_rtti) //create a vertex
          {int h = (int)gwp->canvas->height();
          Tpoint pp((double)p.x(),(double)(h - p.y()));
          v2 = G.NewVertex(pp);ToGrid(v2);
          nodeitem[v2] = CreateNodeItem(v2,gwp);
          mywindow->mouse_actions->ButtonUndoGrid->setDisabled(true);
          if(IsGrid)UpdateSizeGrid();
          }
      else
          v2 = node->v;
      if(v1 != v2) // Create an edge 
          {tedge e = G.NewEdge(v1,v2);
          edgeitem[e] = CreateEdgeItem(e,gwp);
          }
      delete gwp->curs_item;      gwp->curs_item = 0;
      mywindow->information();// Informations
      if(v1 == v2) // We have created a vertex
          PrintSizeGrid();
      return;
      }
  }
QSize GraphEditor::sizeHint() const
  {return QSize(gwp->width(),gwp->height());
  }
void GraphEditor::resizeEvent(QResizeEvent* e)
  {gwp->canvas->setSceneRect(0,0,contentsRect().width(),contentsRect().height());
  if(e->oldSize().width() == e->size().width())
      is_init = true;
  }
void GraphEditor::setsize()
  {resize(sizeHint()); 
  //! Compute the font size
  int fs = (int)((double)Min(gwp->canvas->width(),gwp->canvas->height())/50.); 
  if((fs%2) == 1)++fs; fs = Min(fs,10);
  gwp->fontsize = fs; 
  load(true);
  }

void GraphEditor::load(bool initgrid) 
// by default initgrid = true
// when editing (erasing edges, vertices,reorienting) initgrid = false
  {
  if(!is_init) //MAC
    {gwp->canvas->setSceneRect(0,0,contentsRect().width(),contentsRect().height());
    is_init=true;
    }
  //if(!is_init)return;
  clear();// delete all items
  if(gwp->pGG->nv() > staticData::MaxND)
      {Tprintf("Too big graph nv= %d (>%d)",gwp->pGG->nv(),staticData::MaxND);return;}
  GeometricGraph & G = *(gwp->pGG);

  if(initgrid)
      {Normalise();
      InitGrid(current_grid);
      }
  DrawGrid(current_grid);
  if(ShowGrid)showGrid(true);

  Prop<NodeItem *> nodeitem(G.Set(tvertex()),PROP_CANVAS_ITEM,(NodeItem *)NULL);
  Prop<EdgeItem *> edgeitem(G.Set(tedge()),PROP_CANVAS_ITEM,(EdgeItem *)NULL);
  nodeitem.SetName("nodeitem");edgeitem.SetName("edgeitem");

  for(tvertex v = 1;v <= G.nv();v++)
      nodeitem[v] =  CreateNodeItem(v,gwp);
  for(tedge e = 1;e <= G.ne();e++)
      edgeitem[e] = CreateEdgeItem(e,gwp); 

  if(staticData::ShowExtTbrin())
      {tedge e = G.extbrin().GetEdge();
      EdgeItem *edge = (G.extbrin() > 0) ? edgeitem[e]  : edgeitem[e]->opp;
      edge->SetColor(color[Green],false);
      } 

  Prop<bool> eoriented(G.Set(tedge()),PROP_ORIENTED,false);
  CreateColorItems(gwp,color_node,color_edge);
  G.vcolor.definit(color_node);
  G.vlabel.definit(0L);
  G.elabel.definit(0L);
  CreateThickItems(gwp,width_edge);
  G.ewidth.definit(width_edge);
  }

int GraphEditor::FindItem(QPoint &p,NodeItem* &node,EdgeItem* &edge)
  {int rtt;
  QList<QGraphicsItem *> l=scene()->items(p);
  for(QList<QGraphicsItem *>::Iterator it=l.begin();it!=l.end(); ++it)
      {rtt = (int)(*it)->type();
      if(rtt == node_rtti) 
          {node = (NodeItem *)(*it);
          return rtt;
          }
      else if(rtt == edge_rtti) 
          {edge = (EdgeItem *)(*it);
          return rtt;
          }
      }
  return 0;
  }
int GraphEditor::FindItem(QPoint &p,EdgeItem* &edge)
  {int rtt;
  QRect rect(p.x()-3,p.y()-3,6,6);
  QList<QGraphicsItem *> l=scene()->items(rect);
  for(QList<QGraphicsItem *>::Iterator it=l.begin();it!=l.end(); ++it)
      {rtt = (int)(*it)->type();
      if(rtt == edge_rtti) 
	  {edge = (EdgeItem *)(*it);
	  return rtt;
	  }
      }
  return 0;
  }
int GraphEditor::FindItem(QPoint &p,ColorItem* &coloritem)
  {int rtt;
  QList<QGraphicsItem *> l=scene()->items(p);
  for(QList<QGraphicsItem *>::Iterator it=l.begin();it!=l.end(); ++it)
      {rtt = (int)(*it)->type();
      if(rtt == col_rtti) 
          {coloritem = (ColorItem *)(*it);
          return rtt;
          }
      }
  return 0;
  }
int GraphEditor::FindItem(QPoint &p,ThickItem* &thickitem)
  {int rtt;
  QList<QGraphicsItem *> l=scene()->items(p);
  for(QList<QGraphicsItem *>::Iterator it=l.begin();it!=l.end(); ++it)
      {rtt = (int)(*it)->type();
      if(rtt == thick_rtti) 
          {thickitem = (ThickItem *)(*it);
          return rtt;
          }
      }
  return 0;
  }
void GraphEditor::clear()
  {//erase all the items of the canvas, but not the canvas itself
  if(!scene())return;
  QList<QGraphicsItem *> list = gwp->canvas->items();
  QList<QGraphicsItem *>::Iterator it = list.begin();
  QGraphicsItem * item;
  for (; it != list.end(); ++it)
      {item = (QGraphicsItem *)(*it);
      if(item->scene() == gwp->canvas)
          scene()->removeItem(*it);
      }
  GridDrawn = false;
  }
void GraphEditor::image(QPrinter* printer, QString suffix)
  {if(!scene())return;
  QRect geo = geometry();
  int size = staticData::sizeImage;
  resize(size+4,size+4);
  load(true);
  qApp->processEvents();
  int rtt;
  QList<QGraphicsItem *> list = gwp->canvas->items();
  for(QList<QGraphicsItem *>::Iterator it=list.begin();it!=list.end(); ++it)
      {rtt = (int)(*it)->type();
      if(rtt == col_rtti || rtt == thick_rtti) 
          (*it)->hide();
      }
  int nx = (int)gwp->canvas->width();
  int ny = (int)gwp->canvas->height();
  if(suffix == "png" || suffix == "jpg")
      {QPixmap pixmap = QPixmap(nx,ny);
      QPainter pp(&pixmap);
      pp.setBrush(QBrush(Qt::white));
      pp.setPen(QPen(Qt::white));
      pp.drawRect(QRect(0,0,nx,ny));
      pp.setPen(QPen(Qt::black));
      render(&pp,QRectF(2,2,nx,ny),QRect(2,2,nx,ny));
      pixmap.save(staticData::fileImage);
      }
  else if(suffix == "svg") 
      {QSvgGenerator *svg = new QSvgGenerator();
      svg->setFileName(staticData::fileImage);
      svg->setResolution(90); //ecran pts/pouce
      //svg->setResolution(physicalDpiX()); //ecran 101
      svg->setSize(QSize(nx,ny));
      QPainter pp(svg);
      pp.setBrush(QBrush(Qt::white));
      pp.setPen(QPen(Qt::white));
      pp.drawRect(QRect(0,0,nx,ny));
      pp.setPen(QPen(Qt::black));
      render(&pp,QRectF(2,2,nx,ny),QRect(2,2,nx,ny));
      }
  else if(suffix == "pdf" || suffix == "ps")
      {QPainter pp(printer);
      render(&pp,QRectF(0,0,printer->width(),printer->height()),QRect(2,2,nx,ny));
      }
  setGeometry(geo);
  load(true);
  }
void GraphEditor::print(QPrinter *printer)
  {QPainter pp(printer);
  int nx = (int)gwp->canvas->width();
  int ny = (int)gwp->canvas->height();
  gwp->canvas->render(&pp,QRectF(0,0,printer->width(),printer->height()),QRect(0,0,nx,ny));
  }





