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
#include <TAXI/IntList.h>
#include <TAXI/Tdebug.h>
#include <TAXI/Tmessage.h>
#include <TAXI/color.h>

int DecompMaxBip(TopologicalGraph &G);
int DecompMaxBip(TopologicalGraph &G, tbrin &FirstBrin);
void CalcTotalOrder(GeometricGraph &G, const tbrin FirstBrin);
static int PackFace(TopologicalGraph &G, tbrin b0, svector<int> &marked,
                    tbrin &b_left, tbrin &b_right);


int DecompMaxBip(TopologicalGraph &G)
  {int n_origin = G.nv();
  tbrin FirstBrin;
  int ret = DecompMaxBip(G,FirstBrin);
  for(tvertex v = G.nv(); v > n_origin;v--)
      G.DeleteVertex(v);
  return ret;
  }
int DecompMaxBip(TopologicalGraph &G, tbrin &FirstBrin)
// construct 2-decomposition of the graph {v_1,v_2}-enracin‚.
// v_1 is the root of blue branching.
// v_2 is the root of red branching.
// blue branching turns right on red vertices.
  {if(!G.CheckBipartite())return -4;
  if(G.VertexQuadrangulate() != 0)return -2; 
  FirstBrin = G.extbrin();

  tvertex v1 = G.vin[-FirstBrin];             // leftmost vertex
  tvertex v2 = G.vin[-G.acir[FirstBrin]];     // rightmost vertex

  // add four edges.
  tedge e1 = G.NewEdge(v1,v2);
  tedge e2 = G.NewEdge(v1,v2);
  tedge e3 = G.NewEdge(v2,v1);
  tedge e4 = G.NewEdge(v2,v1);

  // 2-orient.
  if (G.InfOrient()-1 != 2)return -1;

  // delete the four edges.
  G.DeleteEdge(e4);
  G.DeleteEdge(e3);
  G.DeleteEdge(e2);
  G.DeleteEdge(e1);
  // If FirstBrin is positive, InfOrient() must have reoriented it. So...
  if (FirstBrin() > 0) FirstBrin=-FirstBrin;

  Prop<short> vcolor(G.Set(tvertex()),PROP_COLOR);
  Prop<short> ecolor(G.Set(tedge()),PROP_COLOR);

  tvertex v;
  ForAllVertices(v,G) vcolor[v] = 0;
  tedge e;
  ForAllEdges(e,G) ecolor[e] = Red;

  vcolor[v1] = Blue;
  tbrin b, b0;
  IntList stk;
  Forall_adj_brins(b,v1,G)
      {ecolor[b.GetEdge()] = Blue;
      vcolor[G.vin[-b]] = Red;
      stk.push(-b());
      }
  while (!stk.empty())
      {b0 = stk.pop();
      if (vcolor[G.vin[b0]] == Blue)
          {b = G.acir[b0];
          while (b > 0)
              {ecolor[b.GetEdge()] = Blue;
              if (vcolor[G.vin[-b]] == 0)
                  {vcolor[G.vin[-b]] = Red;
                  stk.push(-b());
                  }
              b = G.acir[b];
              }
          }
      else
          {b = G.cir[b0];
          while (b > 0)
              {ecolor[b.GetEdge()] = Blue;
              if (vcolor[G.vin[-b]] == 0)
                  {vcolor[G.vin[-b]] = Blue;
                  stk.push(-b());
                  }
              b = G.cir[b];
              }
          }
      }
  vcolor[v2] = Blue;
  return 0;
  }

static int PackFace(TopologicalGraph &G, tbrin b0, svector<int> &marked,
                    tbrin &b_left, tbrin &b_right)
// b0 est le brin gauche d'une arˆte marqu‚e, incidente … la face … empiler
  {tbrin b = b0;
  IntList left_brins, right_brins;
  Prop<short> ecolor(G.Set(tedge()),PROP_COLOR);

  do {b=-G.cir[b];} while(marked[b.GetEdge()] && b!=b0);
  if (b==b0) return 1;       // packed all faces.

  while (ecolor[b.GetEdge()]==Blue && !marked[b.GetEdge()])
      {left_brins.push(b());
      b = -G.cir[b];
      }
  tvertex v = G.vin[-b];    // the vertex where two edge-colors meet.
  while (ecolor[b.GetEdge()]==Red && !marked[b.GetEdge()])
      {right_brins.push(b());
      b = -G.cir[b];
      }
  if (!marked[b.GetEdge()]) return -2;  // NG

  while (!left_brins.empty()) marked[Abs(left_brins.pop())] = 1;
  while (!right_brins.empty()) marked[Abs(right_brins.pop())] = 1;

  int flag = 3;
  Forall_adj_brins(b,v,G)
      {if (b < 0)
          {if ((flag & 1) && ecolor[b.GetEdge()]==Red)
              {b_right=-b; flag-=1;}
          if ((flag & 2) && ecolor[b.GetEdge()]==Blue)
              {b_left =-b; flag-=2;}
          }
      if(!flag) break;
      }

  return 0;
  }
void CalcTotalOrder(GeometricGraph &G, const tbrin FirstBrin)
// call this function after DecompMaxBip().
  {svector<int> marked(1,G.ne(),0);
  Prop<int> h(G.Set(tvertex()),PROP_DRAW_INT_1);
  marked.SetName("PAGE2:marked");

  //tbrin FirstBrin = G.FindExteriorface();
  tvertex v1 = G.vin[-FirstBrin];
  tvertex v2 = G.vin[-G.acir[FirstBrin]];

  tbrin b_left = G.cir[-FirstBrin];
  tbrin b_right = -G.cir[-b_left];
  marked[b_left.GetEdge()] = 1;
  marked[b_right.GetEdge()] = 1;

  // first 3 vertices.
  h[G.vin[b_left]] = 1;
  h[G.vin[-b_left]] = 2;
  h[G.vin[b_right]] = G.nv();
  int num_packed_v = 2;
    
  // pack faces.
  while (1)
      {if (G.cir(-b_right) == -b_left)      // only one incident face.
          PackFace(G, b_left, marked, b_left, b_right);
      else if (PackFace(G, b_left, marked, b_left, b_right) != 0)
          PackFace(G, -b_right, marked, b_left, b_right);

      num_packed_v++;
      h[G.vin(-b_left)] = num_packed_v;
      if (num_packed_v == G.nv()-1) return;
      }
  }

int EmbedContactBip(GeometricGraph &G)
  {if(!G.CheckBipartite())return -4;
  if(!G.FindPlanarMap())return  -3;
  tbrin FirstBrin;
  int  n_origin = G.nv();
  int m_origin = G.ne();
  PSet1  propSave(G.Set());
  svector<bool> save_oriented(0,G.ne());
  Prop<bool> oriented(G.Set(tedge()),PROP_ORIENTED,false);
  save_oriented.Tswap(oriented);
  if(DecompMaxBip(G,FirstBrin))
      {
//       save_oriented.Tswap(oriented);G.RestoreOrientation();
      return -1;}
  Prop<int> h(G.Set(tvertex()),PROP_DRAW_INT_1);
  h.clear();
  CalcTotalOrder(G,FirstBrin);
  int n_quadrangulate = G.nv();
  tvertex v;
  for(v = G.nv();v > n_origin;v--)
  	G.DeleteVertex(v);
 for(tedge e  =  G.ne();e > m_origin;e--)
  	G.DeleteEdge(e);
 G.Set() =  propSave;
  // computes extremities of vertices
  Prop<int> h1(G.Set(tvertex()),PROP_DRAW_INT_2); 
  Prop<int> h2(G.Set(tvertex()),PROP_DRAW_INT_3); 
  
  for(v = 1;v <= G.nv();v++)
      {h1[v] = n_quadrangulate+1 ;h2[v] = -1;
      }
  tvertex v1,v2;
  for(tedge e = 1;e <= G.ne();e++)
      {v1 = G.vin[e]; v2 = G.vin[-e];
      h1[v1] = Min(h1[v1],h[v2]); h2[v1] = Max(h2[v1],h[v2]);
      h1[v2] = Min(h1[v2],h[v1]); h2[v2] = Max(h2[v2],h[v1]);
      }
  int xymax = h[1];  int xymin = h[1];
  for(v = 2;v <= G.nv();v++)
      {xymax = Max(xymax,h[v]);
      xymin = Min(xymin,h[v]);
      }
  save_oriented.Tswap(oriented);
  G.RestoreOrientation(); // create problem !!
  Prop1<Tpoint> pmin(G.Set(),PROP_POINT_MIN);
  Prop1<Tpoint> pmax(G.Set(),PROP_POINT_MAX);
  pmin() = Tpoint(xymin-1,xymin-1);
  pmax() = Tpoint(xymax+1,xymax+1);
  return 0;
  }


