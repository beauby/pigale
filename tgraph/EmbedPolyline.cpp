/****************************************************************************
**
** Copyright (C) 2001 Hubert de Fraysseix, Patrice Ossona de Mendez.
** All rights reserved.
** This file is part of the PIGALE Toolkit.
**
** This file may be distributed under the terms of the GNU Public License
** appearing in the file LICENSE.HTML included in the packaging of this file.
**
**  ------------- Polyline Drawing algorithm --------------------------------
**  Authors : N. Bonichon, B. Le Saëc and M. Mosbah
** Implementation Author : N. Bonichon
** Reference : N. Bonichon, B. Le Saëc and M. Mosbah, Optimal area algorithm 
** for planar polyline drawings, 28th Graph-Theoretic Concepts in Computer 
** Science (WG'02) in Cesky Krumlov, Czech Republic, LNCS 2573, 35-45, 2002.
*****************************************************************************/

#include <TAXI/Tbase.h>
#include <TAXI/graphs.h>
#include <TAXI/Tmessage.h>
#include <TAXI/Tdebug.h>
#include <TAXI/MaxPath.h>
#include <TAXI/color.h>
#include <TAXI/SchPack.h>

// in pigalePaint.cpp
extern void DrawPolyline(TopologicalGraph &G);


void compute_parents(GeometricGraph &G, short TreeColor, tbrin RootBrin,
                         svector<tbrin> &Father, svector<short> &ecolor)
  {svector<int> marked(1,G.ne(),0);
  marked.SetName("marked");
  tbrin b = RootBrin;
  while (1)
      {do
          {b = G.cir[b];
          if(b == RootBrin)return;
          }while (ecolor[b.GetEdge()] != TreeColor && !marked[b.GetEdge()]);

      if (!marked[b.GetEdge()])     // Montee dans l'arbre
          {Father[G.vin[-b]] = b;
          marked[b.GetEdge()] = 1;
          b = -b;
          }
      else               // Descente
          b = -b;
      }
  }

static void compute_coords(GeometricGraph &G, short TreeColor1, short TreeColor2, tbrin RootBrin,  svector<tbrin> &Father0, svector<tbrin> &Father1, svector<tbrin> &Father2, svector<short> &ecolor, svector<int> &x, svector<int> &y, svector<Tpoint> &Ebend)
  {
  svector<int> marked(1,G.ne(),0); marked.SetName("marked");
  svector<int> Descendants(1,G.nv(),1); Descendants.SetName("Descendants");
  
  tbrin b;
  tvertex u, f0, f1, f2;
  tvertex v0, v1, v2; // roots of T0,T1,T2
  svector<int> Lmax1(1,G.nv(),0);
  svector<int> Lmax2(1,G.nv(),0);
  svector<int> XR(1,G.nv(),0);
  svector<int> XL(1,G.nv(),-1);
  tbrin RootBrin1, RootBrin2;
  RootBrin2 = RootBrin;
  RootBrin1 = -RootBrin;
  b = RootBrin1;
  v0 = G.vin[RootBrin2];
  v1 = G.vin[-RootBrin2];
  v2 = G.vin[-G.acir[-RootBrin2]];
  y[v0] = 0;
  x[v0] = 0;
  x[v2] = 0;

  // Computes ordinates
  while (1)
      {do
          {b = G.cir[b];
          if(b == RootBrin1) break;
          }while (ecolor[b.GetEdge()] != TreeColor1 && !marked[b.GetEdge()]);
      if (b == RootBrin1) break;
      if (!marked[b.GetEdge()])     // Montee dans l'arbre
          {marked[b.GetEdge()] = 1;
          b = -b;
          }
      else               // Descente
          {u = G.vin[b];
          f0 =G.vin[Father0[u]];
          f1 =G.vin[Father1[u]];
          f2 =G.vin[Father2[u]];
          if (f0 != 0) y[u] = Max(y[u],y[f0]+1);
          y[u] = Max(y[u],Min(Lmax1[u],Lmax2[u])+1);
          if (f2 != 0) y[f2] = Max(y[f2], y[u]);
          if (f1 != 0) y[f1] = Max(y[f1], y[u]);
          if (f2 != 0) y[f2] = Max(y[f2], Lmax1[u]+1);
          if (f1 != 0) y[f1] = Max(y[f1], Lmax2[u]+1);
	
          if (f1 != 0)     Lmax1[f1] = Max(Lmax1[f1], y[u]);
          if (f2 != 0)     Lmax2[f2] = Max(Lmax2[f2], y[u]);
          b = -b;
          }
      }
  y[v1] = Max(y[v1], Lmax2[v2]+1);

  // Computes Bends and abscissa
  Fill(marked, 0);
  int current_x=1;
  b = RootBrin2;
  while (1)
      {do
          {b = G.cir[b];
          if(b == RootBrin2)break;
          }while (ecolor[b.GetEdge()] != TreeColor2 && !marked[b.GetEdge()]);
      if (b == RootBrin2) break;
      if (!marked[b.GetEdge()])     // Montee dans l'arbre
	{marked[b.GetEdge()] = 1;
          b = -b;
          }
      else               // Descente
          {u = G.vin[b];
          f0 =G.vin[Father0[u]];
          f1 =G.vin[Father1[u]];
          f2 =G.vin[Father2[u]];
          if (Descendants[u] == 1) 
              {x[u] = current_x;
              XL[u] = current_x;
              XR[u] = current_x;
              current_x++;
              }
          else 
              {if (y[u] == Lmax2[u])
                  x[u] = XR[u];
              else
                  x[u] = XL[u];
              }
          if (f0 != 0) 
              {XR[f0]=XR[u];
              if (XL[f0] == -1)   XL[f0] = XL[u];
              Ebend[b.GetEdge()] = Tpoint(x[u],y[f0]+1);
              }
          if (f1 != 0 && x[u] != XR[u]) 
              Ebend[Father1[u].GetEdge()] = Tpoint(XR[u],y[u]);
          if (f2 != 0 && x[u] != XL[u]) 
              Ebend[Father2[u].GetEdge()] = Tpoint(XL[u],y[u]);
          Descendants[f0] += Descendants[u];
          b = -b;
          }
      }
  x[v1] = current_x;
  Ebend[RootBrin2.GetEdge()] = Tpoint(x[v1], y[v0]+1);
  }
// Compute the Number of leaves in the tree of color TreeColor
static int nb_leaves(GeometricGraph &G, short TreeColor, tbrin RootBrin,
		     svector<short> &ecolor)
  {svector<int> marked(1,G.ne(),0);  marked.SetName("marked");
  svector<bool> is_leaf(1,G.nv(),true);  is_leaf.SetName("is_leaf");
  tbrin b = RootBrin;
  int root_distance = 0;
  int nb_leaves = 0;
  while (1)
      {do
          {b = G.cir[b];
          if(b == RootBrin) return nb_leaves;
          }while (ecolor[b.GetEdge()] != TreeColor && !marked[b.GetEdge()]);

      if (!marked[b.GetEdge()])     // Montee dans l'arbre
          {root_distance++;
          marked[b.GetEdge()] = 1;
          b = -b;
          }
      else               // Descente
          {if (is_leaf[G.vin[b]])
              nb_leaves ++;
          root_distance--;
          is_leaf[G.vin[-b]] = false;
          b = -b;
          }
      }
  return nb_leaves;
  }
int EmbedPolyline(TopologicalGraph &G)
  {if(G.nv() < 3 || G.ne() < 2)return -1;
  int OldNumEdge = G.ne();
  PSet1  propSave(G.Set());
  G.MakeConnected();
  if(!G.FindPlanarMap() )
      {Tprintf("Not Planar Graph");
      for(tedge e = G.ne(); e > OldNumEdge; e--) G.DeleteEdge(e);
      return -1;
      }
 tbrin FirstBrin = 1;
  bool MaxPlanar = (G.ne() != 3 * G.nv() - 6) ? false : true;
  int len;
  if(!FirstBrin && !MaxPlanar)
      G.LongestFace(FirstBrin,len);
  else if(FirstBrin == 0)
      {FirstBrin = G.extbrin();FirstBrin = -G.acir[FirstBrin];}

  if(!MaxPlanar && G.ZigZagTriangulate())return -2;
  svector<short> ecolor(1, G.ne());
  SchnyderDecomp(G,FirstBrin,ecolor);
  GeometricGraph G0(G);
  
  //Compute trees 
  tedge ee;
  Prop<Tpoint> Ebend(G.Set(tedge()),PROP_DRAW_POINT_3);
  
  svector<tbrin> FatherB(1,G.nv(),(tbrin)0);           FatherB.SetName("FatherB");
  svector<tbrin> FatherG(1,G.nv(),(tbrin)0);           FatherG.SetName("FatherG");
  svector<tbrin> FatherR(1,G.nv(),(tbrin)0);           FatherR.SetName("FatherR");
  svector<int> x(1,G.nv(),0), y(1,G.nv(),0);
  x.clear(); y.clear();
  compute_parents(G0, Blue, -FirstBrin, FatherB, ecolor);
  compute_parents(G0, Red, FirstBrin, FatherR, ecolor);
  compute_parents(G0, Green, -G0.acir[FirstBrin], FatherG, ecolor);
  // Compute the number of leaves of each tree
  int nb_leavesB, nb_leavesR, nb_leavesG;
  nb_leavesB = nb_leaves(G0, Blue, -FirstBrin, ecolor);
  nb_leavesR = nb_leaves(G0, Red, FirstBrin, ecolor);
  nb_leavesG = nb_leaves(G0, Green, -G0.acir[FirstBrin], ecolor);

    // Compute the coordinates using the tree with the minimum number of leaves
  ForAllEdges(ee, G) Ebend[ee] = Tpoint(-1,-1);
  if (nb_leavesB <= nb_leavesR && nb_leavesB <= nb_leavesG) 
      compute_coords(G0,Red,Blue,-FirstBrin,FatherB,FatherR,FatherG,ecolor,x,y,Ebend);
  else if (nb_leavesR <= nb_leavesG) 
      compute_coords(G0,Green,Red,G0.acir[FirstBrin],FatherR,FatherG,FatherB,ecolor,x,y,Ebend);
  else 
      compute_coords(G0,Blue,Green,G0.acir[-G0.acir[FirstBrin]],FatherG,FatherB,FatherR,ecolor,x,y,Ebend);
  // computes extremities of vertices
  Prop<Tpoint> Epoint1(G.Set(tedge()),PROP_DRAW_POINT_1);
  Prop<Tpoint> Epoint2(G.Set(tedge()),PROP_DRAW_POINT_2);
  Prop<Tpoint> Vcoord(G.Set(tvertex()),PROP_DRAW_POINT_1);
  G.Set() =  propSave;
  Prop1<Tpoint> pmin(G.Set(),PROP_POINT_MIN);
  Prop1<Tpoint> pmax(G.Set(),PROP_POINT_MAX);  
  tvertex vv;
  pmin() = Tpoint(0,0);
  pmax() = Tpoint(0,0);
  ForAllVertices(vv, G0) 
   {Vcoord[vv] = Tpoint(x[vv], y[vv]);
   if (Vcoord[vv].x() > pmax().x())
       pmax().x() = Vcoord[vv].x();
   if (Vcoord[vv].y() > pmax().y())
       pmax().y() = Vcoord[vv].y();
   }

  ForAllEdges(ee, G) {
    Epoint1[ee] = Vcoord[G.vin[ee]];
    Epoint2[ee] = Vcoord[G.vin[-ee]];
  }
  // delete the edges added by Connexity and Triangulation
  for(tedge e = G.ne();e > OldNumEdge;e--) G.DeleteEdge(e);
  //G.extbrin() = FirstBrin;
  return 0;
  }










