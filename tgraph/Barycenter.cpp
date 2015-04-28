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


#include <TAXI/Tbase.h>
#include <TAXI/graphs.h>
#include <TAXI/Tmessage.h>
#include <TAXI/smatrix.h>

int ComputeBary(GeometricGraph &G,SMatrix &M,svector<double> &x0, svector<double> &y0)
  {if(!G.CheckConnected())return 1;
  Prop<Tpoint> vcoord(G.Set(tvertex()),PROP_COORD);
  svector<double> x(G.nv()),y(G.nv());
  int ok;
  SMatrix Inv = M.Inverse(ok);
  if(!ok)return 2;
  x = Inv * x0;
  y = Inv * y0;

  tvertex v;
  ForAllVertices(v,G)
      vcoord[v] = Tpoint(x[v()-1], y[v()-1]);
  return 0;
  }

int SetTutte(GeometricGraph &G,SMatrix &M, svector<double> &x0, svector<double> &y0)
  {if(!G.CheckConnected())return 1;
  G.RemoveLoops();
  if(G.ComputeGeometricCir())return 2;
  Prop<Tpoint> vcoord(G.Set(tvertex()),PROP_COORD);
  // mark all the vertices incident to the outer face.
  tbrin FirstBrin = G.FindExteriorFace();
  tbrin b = FirstBrin;
  svector<int> marked(1,G.nv(),0);
  do  {marked[G.vin[b]]=1; b=G.cir[-b];} while(b != FirstBrin);
  double d;
  tvertex v;
  M.clear();
  ForAllVertices(v,G)
      if(marked[v])
          {M[v()-1][v()-1]=1;
          x0[v()-1]=vcoord[v].x();
          y0[v()-1]=vcoord[v].y();
          }
      else 
          {M[v()-1][v()-1]=1; 
          d = -1.0/G.Degree(v);
          Forall_adj_brins(b,v,G)
              M[v()-1][G.vin[-b]()-1]=d;
          }
  return 0;
  }
int SetTutteCircle(GeometricGraph &G,SMatrix &M, svector<double> &x0, svector<double> &y0,tbrin FirstBrin)
  {Prop<Tpoint> vcoord(G.Set(tvertex()),PROP_COORD);
  // mark all the vertices incident to the outer face.
  tbrin b = FirstBrin;
  svector<int> marked(1,G.nv(),0);
  int len=0;
  tvertex v;
  x0.clear();
  y0.clear();
  
  do  
      {len++;
      marked[G.vin[b]]=1; 
      b=G.cir[-b];
      } while(b != FirstBrin);
  double angle = 2.*acos(-1.)/len;
  int i=0;
  do
      {v = G.vin[b];
      vcoord[v].x()=100.*cos(i*angle);
      vcoord[v].y()=100.*sin(i*angle);
      i++;
      b = G.cir[-b];
      } while(b != FirstBrin);
  double d;
  M.clear();
  ForAllVertices(v,G)
      if (marked[v])
          {M[v()-1][v()-1]=1;
          x0[v()-1]=vcoord[v].x();
          y0[v()-1]=vcoord[v].y();
          }
      else 
          {M[v()-1][v()-1]=1; 
          d = -1.0/G.Degree(v);
          Forall_adj_brins(b,v,G)
              M[v()-1][G.vin[-b]()-1]=d;
          }
  return 0;
  }
void TutteCircle(GeometricGraph &G, tbrin FirstBrin)
  {SMatrix M(G.nv());
  svector<double> x0(G.nv()),y0(G.nv());
  int res;
  if ((res=SetTutteCircle(G,M,x0,y0,FirstBrin))!=0)
      {Tprintf("Error in SetTutteCircle : %d",res);
      setPigaleError(A_ERRORS_TUTTECIRCLE); 
      return;
      }
  if ((res=ComputeBary(G,M,x0,y0))!=0)        
      {Tprintf("Error in ComputeBarry : %d",res);
      setPigaleError(A_ERRORS_BARYCENTER); 
      return;
      }
  }
