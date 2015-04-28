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
\file CanvasSpring.cpp
\ingroup editor
\brief  Spring embedders
 */

#include "pigaleWindow.h"
#include "GraphWidget.h"
#include "mouse_actions.h"
#include "gprop.h"
//#include <TAXI/Tbase.h> 
#include <QT/pigaleQcolors.h> 
#include <QT/Misc.h> 
#include <QT/clientEvent.h>

#include <QProgressBar>
#ifdef _MSC_VER
#define isnan _isnan
#endif

int ComputeBounds(GeometricGraph &G,double &xmin,double &xmax, double &ymin,double &ymax
		  ,double & dx,double & dy)
  {G.MinMaxCoords(xmin,xmax,ymin,ymax);
  dx = xmax - xmin;
  dy = ymax - ymin;
  return 0;
  } 
char sgn(double f)
{ if (f==0) return('0');
 if (f<0) return('-'); else return('+');}


void GraphEditor::Spring()
  {grabKeyboard(); 
  GeometricGraph & G = *(gwp->pGG);
  Prop<NodeItem *> nodeitem(G.Set(tvertex()),PROP_CANVAS_ITEM);
  svector<Tpoint> translate(1,G.nv()); translate.clear();
  DoNormalise = true;
  int h = (int)gwp->canvas->height();
  int w = (int)gwp->canvas->width();
  double mhw = Min(w,h) - 2*BORDER;
  Tpoint center((w - space - sizerect)/2.,h/2.); 
  int n_red,n = G.nv(),m =G.ne();
  double len,len02 = mhw*mhw/n;
  // during iteration keeep the drawing size
  double expand = 1.;
  double hw,hw0 = .25*(mhw*mhw)/(n*m);//.5
  int iter,niter = 2000;
  //int iter,niter = 1;
  double dist2,strength,dx,dy,dep;
  double xmin,xmax,ymin,ymax,sizex,sizey,sizex0,sizey0;
  Tpoint p0,p,translation;
  double force = 1.;
  int stop = 0;
  // Compute bounds
  ComputeBounds(G,xmin,xmax,ymin,ymax,sizex0,sizey0); 
  
  for(iter = 1;iter <= niter;iter++)
      {translate.clear();
      if(iter > 50)force *= .99;
      else if(iter > 100)force *= .98;
      // Compute mean length of edges
      len = .0;
      for(tedge e = 1; e <= m;e++)
	  {p0 = G.vcoord[G.vin[e]]; p = G.vcoord[G.vin[-e]];
	  len += Distance(p0,p);
	  }
      len /= m;
      len02 = len*len;
      hw = expand*hw0;
      for(tvertex v0 = 1;v0 <= n;v0++)
	  {p0 = G.vcoord[v0];
	  translation.x() = translation.y() = .0;
	  // v0 repulse other vertices (1/d²)
	  for(tvertex v = 1;v <= n;v++) 
	      {if(v == v0)continue;
	      p = G.vcoord[v];
	      dist2 = Max(Distance2(p0,p),1.);
	      strength = (hw/dist2);
	      translation  += (p0 - p)*strength; 
	      }
	  // v0 is repulsed by non adjacent edges (1/d²)
	  // if too small cir changes
	  for(tedge e = 1; e <= m;e++)
	      {tvertex v = G.vin[e], w = G.vin[-e];
	      if(v0 == v || v0 == w)continue;
	      dist2 = dist_seg(p0,G.vcoord[v],G.vcoord[w],p);
	      if(dist2 > 2.)
		  {strength = (hw/dist2); 
		  translation += (p0 - p)*strength;
		  }
	      else if(G.vcoord[v].y() != G.vcoord[v].y())
		  translation.x() += 5.;
	      else
		  translation.y() += 5.;
	      }
	  //v0 is attracted or repulsed by its neighbours (1/d)
	  tbrin b0 = G.pbrin[v0];
	  tbrin b = b0;
	  do
	      {p = G.vcoord[G.vin[-b]];
	      dist2 = Max(Distance2(p0,p),1.);
	      strength = Min(sqrt(hw/dist2),.1);
	      if(dist2 > len02/4)
		  translation  -= (p0-p)*strength*.5;
	      else if(dist2 < 4*len02) 
		  translation  += (p0-p)*strength*.5;
	      }while((b = G.cir[b]) != b0);
	  // v0 is attracted by the center (1/d)
	  dist2 = Max(Distance2(p0,center),1.);
	  strength = Min(sqrt(hw/dist2),.5)*.5;
	  translate[v0] -= (p0 - center)*strength;
	  translation -= (p0 - center)*strength;
	  // update v0
	  translation *= force;
	  translate[v0] = translation;
	  G.vcoord[v0] += translation;
	  }

      // update the drawing
      dep = .0;
      n_red = 0;
      bool redraw = iter%8 == 0;
      for(tvertex v = 1;v <= n;v++)
	  {dx = Abs(translate[v].x()); dy = Abs(translate[v].y());
	  dep = Max(dep,dx);  dep = Max(dep,dy);
	  if(dx > 1. || dy > 1.) 
	      {if(redraw)nodeitem[v]->SetColor(color[G.vcolor[v]]);
	      }
	  else
	      {if(redraw)nodeitem[v]->SetColor(Qt::red);
              ++n_red;
              }
	  if(redraw)nodeitem[v]->moveTo(G.vcoord[v]);
	  }
      //stop = (n_red >= (2*G.nv())/3)? ++stop : 0;
      stop = (n_red == G.nv())? stop+1 : 0;
      if(stop)force *= .9;
      if(dep < .25 || stop == 4)break;
      //hub qApp->processEvents(1);
      qApp->processEvents();
      //if(gwp->mywindow->getKey() == Qt::Key_Escape)break;
      if(key_pressed == Qt::Key_Escape)break;
      // Compute bounds ro adapt the expand factor (should not be too strong)
      ComputeBounds(G,xmin,xmax,ymin,ymax,sizex,sizey); 
      if(sizex > sizex0 && sizey > sizey0)
	  expand /= Max(sizex/sizex0,sizey/sizey0);
      else if(sizex < sizex0 && sizey < sizey0)
	  expand *= Max(sizex0/sizex,sizey0/sizey);
      sizex0 = sizex;      sizey0 = sizey;
      }

  Normalise();
  // same as load(false) but much faster
  for(tvertex v = 1;v <= n;v++)
      {nodeitem[v]->SetColor(color[G.vcolor[v]]);
      nodeitem[v]->moveTo(G.vcoord[v]);
      }
  scene()->update();
#ifdef  VERSION_ALPHA 
  if(debug())
      Tprintf("Iter=%d len=%d stop=%d dep=%f expand=%f force=%f",iter,(int)len,stop,dep,expand,force);
#endif
  releaseKeyboard(); 
  }

//**************************************************************************************
void GraphEditor::SpringPreservingMap(bool draw)
/*
t current tanslation of v0
*/
{
#ifdef TDEBUG
  if(debug())DebugPrintf("Spring PM\n");
#endif
  GeometricGraph & G = *(gwp->pGG);
  Prop<NodeItem *> nodeitem(G.Set(tvertex()),PROP_CANVAS_ITEM);
  svector<int> degree(1,G.nv()); 
  pigaleWindow *mw = mywindow;
  if(!draw) // called by thread
      {progressEvent *event = new progressEvent(1,G.nv());
      QApplication::postEvent(mw,event); 
      Normalise();
      }
  else
      {mw->progressBar->setRange(0,G.nv());
      mw->progressBar->setValue(0);
      mw->progressBar->show();
      setRenderHints(0);
      }
  DoNormalise = true;
  //int option = Twait("option");
  int h = (int)gwp->canvas->height(),w = (int)gwp->canvas->width();
  double mhw = Min(w,h) - 2*BORDER;
  int n_red,n = G.nv(),m =G.ne();
  double len=.0,len02 = mhw*mhw/n;
  // during iteration keeep the drawing size
  double expand = 1.;
  double hw,hw0 = .1*(mhw*mhw)/(n*m);//.25
  //double hw,hw0 = .5*(mhw*mhw)/(n*m);//.5
  int iter,niter = 2000;
  double dist2,strength,dx,dy,dep = .0;
  double xmin,xmax,ymin,ymax,sizex,sizey,sizex0,sizey0;
  Tpoint p0,p,t,tt,center((w - space - sizerect)/2.,h/2.);
  double force = 1.;
  int stop = 0;
  // Compute bounds
  ComputeBounds(G,xmin,xmax,ymin,ymax,sizex0,sizey0);
  // Compute degrees;
  tvertex v;
  for(v = 1;v <= n;v++)
      degree[v] = G.Degree(v);

  for(iter = 1;iter <= niter;iter++)
    {//if((iter) == 0)cout<<"iter:"<<iter<<"  "<<niter<<endl;
      t=Tpoint(0,0);
      if(iter > 100)force *= .99;
      else if(iter > 200)force *= .98;
      // Compute mean length of edges
      len = .0;
      for(tedge e = 1; e <= m;e++)
          {p0 = G.vcoord[G.vin[e]]; p = G.vcoord[G.vin[-e]];
          len += Distance(p0,p);
          }
      len /= m;
      len02 = len*len;
      hw = expand*hw0;
      dep = .0;
      n_red = 0;
      for(tvertex v0 = 1;v0 <= n;v0++)
          {p0 = G.vcoord[v0];
          t.x() = t.y() = .0;
          // v0 repulse other vertices (1/d²)
          for(tvertex v = 1;v <= n;v++) 
              {if(v == v0)continue;
              p = G.vcoord[v];
              tbrin b0 =  G.pbrin[v0];
              tbrin b = b0;
              double count=0;
              Tpoint pmin;
              Tpoint pcenter=Tpoint(0,0);
              if (b0!=0)
                  do
                      if (G.vin[-b]!=v)
                          {double d2=dist_seg(p,p0,G.vcoord[G.vin[-b]],pmin);
                          d2=Max(d2,1E-6);
                          pcenter+=pmin/d2; 
                          count+=1/d2;
                          }
                  while ((b=G.cir[b])!=b0);
              if (count!=0)
                  pcenter /= count;
              else pcenter=p0;
              dist2 = Max(Distance2(pcenter,p),.1); // better than 1E-4
              strength = (hw/dist2);
              t += strength * (pcenter-p);
              }
          //v0 is attracted or repulsed by its neighbours (1/d)
          tbrin b0 = G.pbrin[v0];
          tbrin b = b0;
          do
              {tvertex v=G.vin[-b];
              p = G.vcoord[v];
              dist2 = Max(Distance2(p0,p),.1);
              double pond = .5*(1. + degree[v0]/(degree[v0]+degree[v]));
              strength = Min(sqrt(hw/dist2),1)*pond;
              if(dist2 > len02/4)
                  t -= (p0-p)*strength*.5;
              else if(dist2 < 4*len02) 
                  t += (p0-p)*strength*.5;
              }while((b = G.cir[b]) != b0);
          // v0 is attracted by the center (1/d)
          dist2 = Max(Distance2(p0,center),1.);
          //strength = Min(sqrt(hw/dist2),.5)*.5;
          //t -= (p0 - center)*strength;

          // v0 is repulsed by non adjacent edges (1/d²)
          Tpoint p00 = p0 + t;
          for(tedge e = 1; e <= m;e++)
              {tvertex v = G.vin[e], w = G.vin[-e];
              if(v0 == v || v0 == w)continue;
              dist2 = dist_seg(p00,G.vcoord[v],G.vcoord[w],p);
              if(dist2 > 2.) //2. good value
                  {strength = (hw/dist2)*2.; //better with *2
                  t += (p00 - p)*strength;
                  }
              else
                  {if (dist2!=0)
                      tt=p00-p;
                  else // more diffcult
                      {Tpoint t1=G.vcoord[v]-G.vcoord[w];
                      tt.x()=-t1.y(); tt.y()=t1.x(); // compute vector orthogonal to (v,w)
                      if ((p0-G.vcoord[v])*tt>0) tt=-tt;
                      }
                  if (tt*tt!=0) tt = tt/sqrt(tt*tt); // normalize
                  t += hw/2.*tt; // should not be decrease
                  }
              }
          t *= force;
          // Check crossings
          bool found=false;
          if (t*t!=0)
              {for (tbrin b=1; b<=G.ne(); b++)
                  {tvertex v1=G.vin[b];
                  tvertex v2=G.vin[-b];
                  if (v1==v0 || v2==v0) continue;
                  Tpoint p1=G.vcoord[v1];
                  Tpoint p2=G.vcoord[v2];
                  double d1=Determinant(p1-p0,t);
                  double d2=Determinant(p2-p0,t);
                  double d3=Determinant(p0-p1,p2-p1);
                  double d4=Determinant(p0+t-p1,p2-p1);
                  double x1 = d1*d2; double x2=d3*d4;
                  if ((x1<=0 && x2<0) || (x1<0 && x2<=0))
                      {found=true; break; }
                  }
              if (!found)
                  {tbrin b0=G.pbrin[v0];
                  tbrin b=b0;
                  bool dobrk=false;
                  do 
                      {Tpoint pp0=G.vcoord[G.vin[-b]];
                      for (tvertex z=1; z<=G.nv(); z++)
                          {if (z==v0 || z==G.vin[-b]) continue;
                          Tpoint p=G.vcoord[z];
                          double d1=Determinant(p0-pp0,p-pp0);
                          double d2=Determinant(p0+t-pp0,p-pp0);
                          double d3=Determinant(pp0-p0,p-p0);
                          double d4=Determinant(t,p-p0);
                          double x1 = d1*d2; double x2=d3*d4;
                          if ((x1<0 && x2<=0) || (x1<=0 && x2<0))
                              {	dobrk=true; break;}
                          }
                      if (dobrk) {found=true; break;}
                      } while ((b=G.cir[b])!=b0);
                  }
              if(found)
                  t=Tpoint(0,0);
              }
          bool redraw = draw && iter%8 == 0;
          if(!found)
              {G.vcoord[v0] += t; 
              dx = Abs(t.x()); dy = Abs(t.y());
              dep = Max(dep,dx,dy);  
              if (dx > 30./n || dy > 30./n)
		{if(redraw)nodeitem[v0]->SetColor(color[G.vcolor[v0]]);}
              else
		{if(redraw) nodeitem[v0]->SetColor(Qt::red);
                ++n_red;
		}
              if(redraw)nodeitem[v0]->moveTo(G.vcoord[v0],5.);
              }
          else
	    {if(draw)nodeitem[v0]->SetColor(Qt::blue);
            ++n_red;
	    }
          }
      // update the drawing
      if(!draw)
          {progressEvent *event = new progressEvent(0,n_red);
          QApplication::postEvent(mw,event);
          }
      else
          {mw->progressBar->setValue(n_red);
          qApp->processEvents(); // absolutely needed
          }
      stop = (n_red == G.nv())? stop+1 : 0;
      //if(stop)force *= .95;
      if(dep < .1 || stop == 4)break;
      if(mywindow->getKey() == Qt::Key_Escape)break;
      // Compute bounds ro adapt the expand factor (should not be too strong)
      ComputeBounds(G,xmin,xmax,ymin,ymax,sizex,sizey); 
      if(sizex > sizex0 && sizey > sizey0)
          expand /= Max(sizex/sizex0,sizey/sizey0);
      else if(sizex < sizex0 && sizey < sizey0)
          expand *= Max(sizex0/sizex,sizey0/sizey);
      sizex0 = sizex;      sizey0 = sizey;
      }
  Normalise();
  if(!draw)
    {progressEvent  * event = new progressEvent(-1);
      QApplication::postEvent(mw,event);
    }
   else
       {mw->progressBar->hide();
	 setRenderHints(QPainter::Antialiasing);
       // same as load(false) but much faster
       for(v = 1;v <= n;v++)
           {nodeitem[v]->SetColor(color[G.vcolor[v]]);
           nodeitem[v]->moveTo(G.vcoord[v]);
           }
       }
#ifdef  VERSION_ALPHA 
  if(debug())
      Tprintf("Iter=%d len=%d stop=%d dep=%f expand=%f force=%f",iter,(int)len,stop,dep,expand,force);
#endif
#ifdef TDEBUG
     if(debug())DebugPrintf("    END Spring PM\n");
#endif
  }


//******************** JACQUARD SPRING EMBEDDER 
//  ****************** VARIABLES D'EQUILIBRAGE

int isInf (double x)
  {if ( x <= 0.0000001 )return 1;
  return 0;
  }


// l'ordre minimum de deux sommets d'une arete pour qu'un sommet
// virtuel soit ajouter (si l'option est selectionnee i.e
// si addVirtualVertex est vrai)
double coefDiff = 0.707;
int DEFAULT_ADD_VIRTUAL_DEGRE = 4;
int MAX_GENERATIONS = 1000;
double K_ANGLE = 10;
//double K_MINDIST = 100;
double K_MINDIST = 50;
double K_ELECTRO = 10;
double K_SPRING = 0.1;
bool K_FRONTIER = false;
double _4PI2 = 4*PI*PI;
struct tk {double angle,spring,mindist,electro;bool frontier;};

Tpoint ZERO = Tpoint();

/**************************************************************
**                Equilibrage de la Carte                    **
**************************************************************/

/* pour les pottentiels
** on somme les interactions entre les sommets
** et les aretes chargees lineiquement
** l'entier est pour la ponderation
*/

/* isCrossing
** verifie que deux edges, donnés chacun par un de leurs brins, se coupent
*/

static bool isIntersecting(Tpoint &p1, Tpoint &p2, Tpoint &p3, Tpoint &p4)
  { double d1,d2,d3,d4;
  d1 = Determinant(p2 - p1, p4 - p1);
  d2 = Determinant(p2 - p1, p3 - p1);
  d3 = Determinant(p1 - p3, p4 - p3);
  d4 = Determinant(p2 - p3, p4 - p3);
  if ( (d1 == 0) && ((p4.x()-p2.x())*(p4.x()-p1.x())<=0) && ((p4.y()-p2.y())*(p4.y()-p1.y())<=0) )
      {return true;}
  if ( (d2 == 0) && ((p3.x()-p2.x())*(p3.x()-p1.x())<=0) && ((p3.y()-p2.y())*(p3.y()-p1.y())<=0) )
      {return true;}
  if ( (d3 == 0) && ((p1.x()-p4.x())*(p1.x()-p3.x())<=0) && ((p1.y()-p4.y())*(p1.y()-p3.y())<=0) )
      {return true;}
  if ( (d4 == 0) && ((p2.x()-p4.x())*(p2.x()-p3.x())<=0) && ((p2.y()-p4.y())*(p2.y()-p3.y())<=0) )
      {return true;}
  if (( ((d1<0) && (d2>0)) || ((d1>0) && (d2<0)) ) &&
      ( ((d3<0) && (d4>0)) || ((d3>0) && (d4<0)) ) )
      {return true;}
  else
      {return false;}
  }
static int isCrossing(tbrin e, tbrin f, GeometricGraph &G)
  {return isIntersecting(G.vcoord[G.vin[e]], G.vcoord[G.vin[-e]],G.vcoord[G.vin[f]], G.vcoord[G.vin[-f]]);}


/* getAngle brinOrderOk
** getAngle rend l'angle d'un brin entre 0 et 2PI
** brinOrderOk verifie que l'orde de trois brins successifs
** est le meme en coordonnees que celui de la permutation
*/

static double getAngle(tbrin b,GeometricGraph &G)
  {double tan_angle;
  double angle;
  double x1= (G.vcoord[G.vin[b]]).x();
  double y1= (G.vcoord[G.vin[b]]).y();
  double x2= (G.vcoord[G.vin[-b]]).x();
  double y2= (G.vcoord[G.vin[-b]]).y();
    
  if(x1 == x2) 
      {if (y1>y2)
          {return 3*PI/2;} 
      else
          {return PI/2;}
      }
  else
      {tan_angle= (y2-y1)/(x2-x1);
      angle= atan(tan_angle);
      if (x1>x2)
          {return angle+PI;}
      else
          {if (y1>y2) 
              {return angle+2*PI;}
          else
              {return angle;}
          }
      }
  }

/* brinOrderOk
** teste que le brin, son predecesseur et son successeur
** on conserve un ordre normal
*/

static bool brinOrderOk(GeometricGraph &G,tbrin b)
  {int deg = G.Degree(G.vin[b]);
  // b est le brin du milieu
  if (deg == 1)
      {return true;}
  if (deg == 2)
      {double i = getAngle(G.acir[b], G);
      double j = getAngle(b, G);
      if (i == j)
          {return false;}
      else
          {return true;}
      }
  // on a au moins trois brins
  double i = getAngle(G.acir[b],G);
  double j = getAngle(b,G);
  double k = getAngle(G.cir[b],G);
    
  if ( ((i<j) && (j<k)) || ((j<k) && (k<i)) || ((k<i) && (i<j)) )
      {return true;}
  else
      {return false;}
  }


/* CrossingEdgesGraph
** Graphe reliant les aretes faisant partie d'une
** meme face dans le graphe de référence
*/
GraphContainer * TopologicalGraph::CrossingEdgesGraph()
  {if(!FindPlanarMap() )
      {DebugPrintf("Error Computing the CEG:no planar map");
      return (GraphContainer *)0;
      }
    
  int m = ne();
  tedge e,ee;
  tbrin b,fb;
  tvertex v;
  GraphContainer &CEG(*new GraphContainer);
  CEG.setsize(m,0);
  Prop1<tstring> title(Set(),PROP_TITRE);
  Prop1<tstring> titleCEG(CEG.Set(),PROP_TITRE);
  titleCEG() = "CEG-" + title();
  Prop<tvertex> evin(CEG.PB(),PROP_VIN);    evin.clear();
  svector<tedge> voisin(0,m);             voisin.clear();

  ForAllEdgesOfG(e)
      {v = tvertex(e());
      fb = FirstBrin(vin[e]);
      b = fb;
      do
          {voisin[b.GetEdge()]=e;
          b=cir[b];
          }
      while (b!=fb);
      fb = FirstBrin(vin[-e]);
      b = fb;
      do
          {voisin[b.GetEdge()]=e;
          b=cir[b];
          }
      while (b!=fb);
      b=-cir(e);
      ee=b.GetEdge();
      while (ee!=e)
          {if ((ee>e) && (voisin[ee]!=e))
              {CEG.incsize(tedge());
              tbrin eb = CEG.ne(); 
              evin[eb] = v;
              evin[-eb] = tvertex(ee());
              voisin[ee]=e;
              }
          b=-cir(b);
          ee=b.GetEdge();
          }
      b=-cir(-e);
      ee=b.GetEdge();
      while (ee!=e)
          {if ((ee>e) && (voisin[ee]!=e))
              {CEG.incsize(tedge());
              tbrin eb = CEG.ne(); 
              evin[eb] = v;
              evin[-eb] = tvertex(ee());
              voisin[ee]=e;
              }
          b=-cir(b);
          ee=b.GetEdge();
          }
      }
  return &CEG;
  }

/* canMove
** verifie que le deplacement d'un vertex est valide
** pas d'intersection ou d'intervertion de edges
*/
static bool canMove(tvertex v, Tpoint Offset, TopologicalGraph &CEG, GeometricGraph &G)
  {
  /* on verifie d'abord que le vertex n'est pas
  ** superpose a un autre ou qu'il n'y a pas
  ** d'intervertion
  */

  tbrin premierbrin = G.FirstBrin(v);
  G.vcoord[v] += Offset;
  tbrin b,b2,fb;
  b = premierbrin;
  do
      {if (!brinOrderOk(G,-b))
          {G.vcoord[v] -= Offset;
          return false;
          }
      b = G.cir[b];
      }
  while (b!=premierbrin);

  b = premierbrin;
  // et on verifie l'ordre des aretes de la face
  double angle = getAngle(b,G);
  double angleNew;
  double angle0 = angle;
  bool desc = false; // si on a plus d'une descente -> inversion
  b= G.cir[b];

  while (b != premierbrin)
      {angleNew = getAngle(b,G);
      if (angle == angleNew)
          {G.vcoord[v]  -= Offset;
          return false;
          }
      if (!desc)
          {if (angleNew < angle)
              {desc = true;
              if (angleNew >= angle0)
                  {G.vcoord[v] -= Offset;
                  return false;
                  }
              }
          }
      else
          {if ( (angleNew < angle) || (angleNew >= angle0) )
              {G.vcoord[v] -= Offset;
              return false;
              }
    
          }
      angle = angleNew;
      b= G.cir[b];
      }


  // puis les intersections des edges precalcules
  b = premierbrin;
  do
      {fb = CEG.FirstBrin(tvertex(Abs(b())));
      b2 = fb;
      do
          {if (isCrossing(b,tbrin(CEG.vin[-b2]()),G))
              {G.vcoord[v] -= Offset;
              return false;
              }
          b2 = CEG.cir[b2];
          }
      while (b2!=fb);
      b=G.cir[b];
      }
  while (b!=premierbrin);

  G.vcoord[v] -= Offset;
  return true;
  }


/* potentiel
** calcul le potentiel d'un sommet qui viens de bouger
** pour chaque arete incidente, on somme les potentiels des aretes 
** avec les sommets de sa face
*/
static double potEdge(tvertex const &v, tbrin const &brin, tk &k, GeometricGraph &G)
  {Tpoint a,b,c;
  c = (G.vcoord[v]);
  a = (G.vcoord[G.vin[brin]]);
  b = (G.vcoord[G.vin[-brin]]);
    
  double ab = Distance(a,b);
  double h = fabs(Determinant(c-a,b-a))/ab;
  // on donne a h une valeur minimum
  if ( h < 0.0001 )
      {h = 0.0001;}
  double la = ((b-a)*(c-a))/h/ab;
  double lb = ((b-a)*(c-b))/h/ab;
  // minDist vaut la distance minimal de l'arete au sommet
  double minDist;
  if (la*lb < 0)
      {minDist = h;}
  else
      {double bc = Distance(b,c);
      double ac = Distance(a,c);
      minDist = bc<ac? bc: ac;
      }
  if ( minDist < 0.001)
      {minDist = 0.001;}

  double electro = log((la+sqrt(1+la*la))/(lb+sqrt(1+lb*lb)));
  return   (k.mindist/minDist + k.electro*electro);
  }

/* Vis_a_vis_Graph
** Graphe reliant les sommets aux aretes non incidentes mais faisant partie d'une
** meme face dans le graphe de référence
*/


GraphContainer * TopologicalGraph::Vis_a_vis_Graph()
  {if(!FindPlanarMap())
      {DebugPrintf("Error Computing the adj:no planar map");
      return (GraphContainer *)0;
      }
    
  int n = nv();
  int m = ne();
  int nn = n+m;
    
  tedge e;
  tbrin b;
  tbrin Vavb;
  tvertex v;
  tvertex vv;
  GraphContainer &Vav(*new GraphContainer);
  Vav.setsize(nn,0);
  Prop1<tstring> title(Set(),PROP_TITRE);
  Prop1<tstring> titleVav(Vav.Set(),PROP_TITRE);
  titleVav() = "Vis_a_vis-" + title();
  Prop<tvertex> vvin(Vav.PB(),PROP_VIN);    vvin.clear();
  svector<tedge> LastEdge(0,nv());        LastEdge.clear();

  ForAllEdgesOfG(e)
      {LastEdge[vin[e]]=e;
      LastEdge[vin[-e]]=e;
      vv = tvertex(e()+ n);
      b = -cir(e);
      v= vin[b];
      while (b.GetEdge()!=e)
          {if (LastEdge[v]!=e)
              {Vav.incsize(tedge());
              tbrin vb = Vav.ne(); 
              vvin[vb] = v;
              vvin[-vb] = vv;
              LastEdge[v]=e;
              }
          b = -cir(b);
          v= vin[b];
          }
      b = -cir(-e);
      v = vin[b];
      while (b.GetEdge()!=e)
          {if (LastEdge[v]!=e)
              {Vav.incsize(tedge());
              tbrin vb = Vav.ne(); 
              vvin[vb] = v;
              vvin[-vb] = vv;
              LastEdge[v]=e;
              }
          b = -cir(b);
          v= vin[b];
          }
      }
  return &Vav;
  }
static int init_extvertex(GeometricGraph &G, svector<int> &ev)
  {tbrin b,fb;
  int nb = 0;
  G.ComputeGeometricCir();
  ev.clear();
  fb = G.FindExteriorFace();
  b = fb;
  do
      {tvertex v = G.vin[b];
      ev[v]++;
      if (ev[v] == 1)nb++;
      b= G.cir[-b];
      }while (b!=fb);
  return nb;
  }

static double potentiel(tvertex v, Tpoint Offset, tk &k, GeometricGraph &G,TopologicalGraph &Vav,svector<int> extvertex)
  {tbrin b,b2,fb,fb2;
  tedge e;
  int n = G.nv();
  G.vcoord[v] += Offset;
  double pot = 0;

  // on somme les potentiels des edges
  fb = Vav.FirstBrin(v);
  b = fb;
  do
      {pot += potEdge(v,(tbrin)(Vav.vin[-b]()-n),k,G);
      b = Vav.cir[b];
      }
  while (b!=fb);
  fb2 = G.FirstBrin(v);
  b2 = fb2;
  do
      {tvertex w = tvertex(Abs(b2())+n);
      fb = Vav.FirstBrin(w);
      b = fb;
      do
          {pot += potEdge(Vav.vin[-b],b2,k,G);
          b = Vav.cir[b];
          }
      while (b!=fb);
      b2 = G.cir[b2];
      }
  while (b2!=fb2);
    
  if (isnan(pot)) {DebugPrintf("potentiel arete-sommet =%f pour v=%d",pot,v());}
    
  // potentiel de type ressort
  if(extvertex[v] != 1)
      {double carre=0;
      double lastAngle = getAngle(G.FirstBrin(v),G);
      double newAngle,diffAngle,angle;
      Forall_adj_brins(b,v,G)
          {Tpoint p1=G.vcoord[v];
          Tpoint p2=G.vcoord[G.vin[-b]];
          carre += Distance2(p1,p2);
          if (G.cir[-b]!=-b)
              {// le premier angle en face
              angle = getAngle(-b,G);
              diffAngle = getAngle(G.cir[-b],G)-angle;
              if ( diffAngle < 0 )
                  {diffAngle += 2*PI;}
              if ( diffAngle != 0 )            
                  // si -b = cir(-b), diffAngle = 0
                  {pot += k.angle/diffAngle;
                  // le second angle en face
                  diffAngle = angle-getAngle(G.acir[-b],G);
                  if ( diffAngle < 0 )
                      {diffAngle += 2*PI;}
                  pot += k.angle/diffAngle;
                  }
              // termine. Calcul du sommet actuel
              }
          newAngle = getAngle(G.cir[b],G);
          diffAngle = newAngle-lastAngle;
          if ( diffAngle < 0 )
              {diffAngle += 2*PI;}
          if ( diffAngle != 0 )
              {pot += k.angle/diffAngle;}
          lastAngle = newAngle;
          }
      if (isnan(pot)) {DebugPrintf("potentiel repulsif des angles=%f pour v=%d",pot,v());}
      pot +=  k.spring*carre;
      if (isnan(pot)) {DebugPrintf("potentiel attractif des sommets=%f pour v=%d",pot,v());}
      }
	  
  G.vcoord[v] -= Offset;
  return pot;
  }

/* tryMove
** rend le carre de la distance parcourue par le sommet 
** en suivant le potentiel
** si ce nombre est nul le sommet est bloqué
*/

static double tryMove(tvertex v, svector<Tpoint> &sumDep, tk &k, TopologicalGraph &Vav, TopologicalGraph &CEG, svector<int> extvertex, GeometricGraph &G)
  {Tpoint dMin;
  Tpoint dTotal = ZERO;
  bool moved;
  double MinPot;
  double tmp,tmp1;
  // on réduit sumDx sumDy par un coef
  sumDep[v] *= coefDiff;
  Tpoint diff;
  Tpoint mov;
  double scale;
  Tpoint xplusun = Tpoint(0.1,0);
  Tpoint yplusun = Tpoint(0,0.1);

  while (true)
      {MinPot = potentiel(v,ZERO,k,G,Vav,extvertex);
      if (canMove(v,xplusun,CEG,G) )
          {diff.x() = potentiel(v,xplusun,k,G,Vav,extvertex) - MinPot;}
      else
          {if( canMove(v,-xplusun,CEG,G))
              {diff.x() = MinPot - potentiel(v,-xplusun,k,G,Vav,extvertex);}
          else
              {diff.x() = 0;}
          }
      if(canMove(v,yplusun,CEG,G))
          {diff.y() = potentiel(v,yplusun,k,G,Vav,extvertex) - MinPot;}
      else
          {if(canMove(v,-yplusun,CEG,G))
              {diff.y() = MinPot - potentiel(v,-yplusun,k,G,Vav,extvertex);}
          else
              {diff.y() = 0;}
          }
      // simplement une normalisation exacte (le max vaut 1)
      if (fabs(diff.x())<1E-6) diff.x()=0;
      if (fabs(diff.y())<1E-6) diff.y()=0;      
      tmp1 = Distance(diff,ZERO);
      if (tmp1 < 1E-6) {return 0;}
      if (isInf(diff.x())) {mov = -(double)isInf(diff.x()) * xplusun;}
      else if (isInf(diff.y())) {mov = -(double)isInf(diff.y()) * yplusun;}
      else mov = ( -diff / tmp1);
      scale = 1;
      tmp = MinPot;
      moved = false;
      while ( canMove(v,(mov * scale),CEG,G) && ( ( tmp1 = potentiel(v,(mov * scale),k,G,Vav,extvertex) ) < tmp ) )
          {tmp = tmp1;
          moved = true;
          scale *= 2;
          }
      if (moved)
          {scale /= 2;
          G.vcoord[v] += scale*mov;
          dTotal += scale*mov;
          }
      else
          {break;}
      }

  // methode classique de mouvement a la fin
  moved = true;
  while (moved)
      {dMin = ZERO;
      if (canMove(v,xplusun,CEG,G) && (MinPot>(tmp = potentiel(v,xplusun,k,G,Vav,extvertex))) )
          {dMin = xplusun;
          MinPot = tmp;
          }
      if(canMove(v,-xplusun,CEG,G) && (MinPot>(tmp = potentiel(v,-xplusun,k,G,Vav,extvertex))) )
          {dMin = -xplusun;
          MinPot = tmp;
          }
      if  ( canMove(v,yplusun,CEG,G) && (MinPot>(tmp = potentiel(v,yplusun,k,G,Vav,extvertex))) )
          {dMin = yplusun;
          MinPot = tmp;
          }
      if  ( canMove(v,-yplusun,CEG,G) && (MinPot>(tmp = potentiel(v,yplusun,k,G,Vav,extvertex))) )
          {dMin = -yplusun;
          MinPot = tmp;
          }
      if (dMin == ZERO)
          {moved = false;}
      else
          {G.vcoord[v] += dMin;
          dTotal += dMin;
          // on avance dans la meme direction jusqu'a ce qu'on remonte ou bloque
          while ( canMove(v,dMin,CEG,G) && (MinPot>(tmp = potentiel(v,dMin,k,G,Vav,extvertex))) )
              {G.vcoord[v] += dMin;
              dTotal += dMin;
              MinPot = tmp;
              }
          }
      }
  sumDep[v] += dTotal;

  return dTotal.x()*dTotal.x()+dTotal.y()*dTotal.y();
  }
//#define isnan isNaN
int GraphEditor::SpringJacquard()			
  {int maxgen      = MAX_GENERATIONS;
  double k_angle   = K_ANGLE;
  double k_mindist = K_MINDIST;
  double k_electro =  K_ELECTRO;
  double k_spring  = .1;
  bool k_frontier  = false;
  int n_red;
  GeometricGraph & G = *(gwp->pGG);
  if(G.ComputeGeometricCir() == 0)
      G.extbrin() = G.FindExteriorFace();
  else
      {Tprintf("NOT A PLANAR EMBEDDING");return -1;}
  DoNormalise = true;
  Prop<NodeItem *> nodeitem(G.Set(tvertex()),PROP_CANVAS_ITEM);
  svector<Tpoint> SumDep(1,G.nv());       
  svector<int> extvertex(1,G.nv());      extvertex.clear();
  svector<bool> blockedvertex(1,G.nv());  blockedvertex.clear();
  TopologicalGraph VAV(*G.Vis_a_vis_Graph());
  TopologicalGraph CEG(*G.CrossingEdgesGraph());
  tvertex v;
  int generations = 0;
  tk k;
  k.angle = k_angle;
  k.mindist = k_mindist;
  k.electro = k_electro;
  k.spring = k_spring;
  k.frontier = k_frontier;
  // initialisation
  int nonblocked = init_extvertex(G,extvertex);
  for(v = 1;v <= G.nv();v++)blockedvertex[v]=false;
  double deplacement = 0;

  //Redimentionnement de la carte
  Normalise();
  Prop<Tpoint> scoord(G.Set(tvertex()),PROP_CANVAS_COORD);
  // Boucle de calcul
  for(;nonblocked && (generations<maxgen);generations++)
      {scoord = G.vcoord;
      for(v = 1;v <= G.nv();v++)// calcul d'une generation
          {if ((k.frontier && extvertex[v]) || (blockedvertex[v]))
              {continue;}
          if (isnan(G.vcoord[v].x()) || isnan(G.vcoord[v].y())) 
	      {DebugPrintf("Error in Jacquard: Emergency exit at v=%d before moving",v());
	      //gwp->mywindow->blockInput(false);
	      return generations;
	      }
          deplacement = tryMove(v,SumDep,k,VAV,CEG,extvertex,G);
          if (isnan(G.vcoord[v].x()) || isnan(G.vcoord[v].y()))
	      {DebugPrintf("Error in Jacquard: Emergency exit at v=%d",v());
	      //gwp->mywindow->blockInput(false);
	      return generations;
	      }
          if (extvertex[v] && deplacement<0.01)
              {blockedvertex[v] = true;
              nonblocked--;
	      nodeitem[v]->SetColor(Qt::red);
              }
          }
      // size_and_center(centre,*this);
      Normalise();
      // Affichage du graphe
      double dx,dy;
      n_red = 0;
      for(tvertex v = 1; v <= G.nv();v++)
	  {dx = G.vcoord[v].x()- scoord[v].x();
	  dy = G.vcoord[v].y()- scoord[v].y();
	  if(Abs(dx) > 1. || Abs(dy) > 1.)
	      {nodeitem[v]->moveBy(dx,-dy);
	      nodeitem[v]->SetColor(color[G.vcolor[v]]);
	      }
	  else
	      {nodeitem[v]->SetColor(Qt::red);++n_red;}
	  }      
      if(n_red > (2*G.nv())/3)break;
      //hub qApp->processEvents(1);
      qApp->processEvents();
      if(mywindow->getKey() == Qt::Key_Escape)break;
      scene()->update(); 
      }
  Tprintf("Spring iter=%d",generations);
  DoNormalise = true;
  Normalise();load(false);
  return generations;
  }


