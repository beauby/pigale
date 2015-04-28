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
#include <QT/Misc.h> 
#include <QT/Action_def.h> 
#include "GraphWidget.h"

using namespace std;

int EmbedCurves(TopologicalGraph &G)
  {
  GeometricGraph GG(G); 
  Prop<Tpoint> vcoord(G.Set(tvertex()),PROP_DRAW_POINT_1);
  int n=G.nv();
  int m=G.ne();
  GraphContainer &NGC = * new GraphContainer();
  TopologicalGraph NG(NGC);
  GeometricGraph NGG(NGC);
  int i;
  for (i=1; i<=n; i++)
    {NG.NewVertex();
    NGG.vcoord[i]=GG.vcoord[i];}
  for (i=1; i<=m; i++)
    {Tpoint p=(GG.vcoord[G.vin[i]]+GG.vcoord[G.vin[-i]])*.5;
    Tpoint vo; vo.x()=-GG.vcoord[G.vin[i]].y()+GG.vcoord[G.vin[-i]].y();
    vo.y()=GG.vcoord[G.vin[i]].x()-GG.vcoord[G.vin[-i]].x();
    vo=vo/sqrt(vo.x()*vo.x()+vo.y()*vo.y());
    NG.NewVertex();
    NGG.vcoord[n+2*i-1]=p+vo*1E-4;
    NG.NewVertex();
    NGG.vcoord[n+2*i]=p-vo*1E-4;
    }
  tedge e;
  for (e=1; e<=m; e++)
    {tvertex s=G.vin[e];
    tvertex t=G.vin[-e];
    NG.NewEdge(s,tvertex(n+2*e()-1));
    NG.NewEdge(t,tvertex(n+2*e()-1));
    NG.NewEdge(s,tvertex(n+2*e()));
    NG.NewEdge(t,tvertex(n+2*e()));
    }
  GeometricGraph *pGG = GetpigaleWindow()->gw->pGG;
  GetpigaleWindow()->gw->pGG=&NGG;
  GetpigaleWindow()->gw->editor->SpringPreservingMap(false);
  GetpigaleWindow()->gw->pGG=pGG;
  tvertex v;
  for (v=1; v<=n; v++)
    vcoord[v]=NGG.vcoord[v];
  Prop<Tpoint> Epoint1(G.Set(tedge()),PROP_DRAW_POINT_1);
  Prop<Tpoint> Epoint2(G.Set(tedge()),PROP_DRAW_POINT_2);
  Prop<Tpoint> Epoint3(G.Set(tedge()),PROP_DRAW_POINT_3);
  Prop1<Tpoint> pmin(G.Set(),PROP_POINT_MIN);
  Prop1<Tpoint> pmax(G.Set(),PROP_POINT_MAX);  
  Tpoint alpha,beta,tmp;
  pmax()=pmin()=NGG.vcoord[1];
  for (v=1; v<=NG.nv(); v++)
    {if (NGG.vcoord[v].x()>pmax().x()) pmax().x()=NGG.vcoord[v].x();
    else if (NGG.vcoord[v].x()<pmin().x()) pmin().x()=NGG.vcoord[v].x();
    if (NGG.vcoord[v].y()>pmax().y()) pmax().y()=NGG.vcoord[v].y();
    else if (NGG.vcoord[v].y()<pmin().y()) pmin().y()=NGG.vcoord[v].y();
    }
  for (e=1; e<=m; e++)
    { Tpoint x = NGG.vcoord[G.vin[e]];
    Tpoint y=NGG.vcoord[G.vin[-e]];
    Tpoint vec=y-x;
    Tpoint ovec; ovec.x()=-vec.y(); ovec.y()=vec.x();
    alpha = NGG.vcoord[n+2*e()-1];
    beta = NGG.vcoord[n+2*e()];
    if (Abs(Determinant(alpha-x,vec))>Abs(Determinant(beta-x,vec)))
      {tmp=alpha; alpha=beta; beta=tmp;}
    double a=0,aa=0,b=0;
    bool ok;
    // check if [x,y] intersects [alpha,beta]
    ok=intersect(x,vec,alpha,beta-alpha,a,b);
    if ((a>=0) && (a<=1) && (b>=0) && (b<=1))
      {
	Epoint1[e]=Epoint2[e]=Epoint3[e]=Tpoint(0,0);}
    else
      { Epoint2[e]=alpha;
      // find intersections of a parallel of (x,y) through alpha and [x,beta] (resp. [y,beta])
      ok=intersect(alpha,vec,x,beta-x,a,b); 
      intersect(alpha,vec,x,ovec,aa,b);
      if (a*aa>=0 && Abs(aa)<Abs(a)) a=aa;
      Tpoint mvec; mvec.x()=x.y()-alpha.y(); mvec.y()=alpha.x()-x.x();
      if (intersect(alpha,vec,(x+alpha)*0.5,mvec,aa,b))
	if (a*aa>=0 && Abs(aa)<Abs(a)) a=aa;
      Epoint1[e]=alpha+vec*a;
      ok=intersect(alpha,vec,y,beta-y,a,b);
      intersect(alpha,vec,y,ovec,aa,b);
      if (a*aa>=0 && Abs(aa)<Abs(a)) a=aa; 
      Epoint3[e]=alpha+vec*a;
      }
    }
  delete &NGC;
  return 0;
  }


