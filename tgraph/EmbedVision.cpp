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
#include <TAXI/Tdebug.h>
#include <TAXI/MaxPath.h>
#include <TAXI/color.h>

// in DrawVision.cpp
extern void DrawVision(TopologicalGraph &G);

static int ComputeExtremities(TopologicalGraph &G,svector<int> &x, 
                              svector<int> &x1,svector<int> &x2,int morg);

bool CheckBipolarlyOriented(TopologicalGraph &G,tvertex &s,tvertex &t,bool &stConnected ,tbrin &bs,bool addEdge);
bool CheckVertexBelongFace(TopologicalGraph &G,tvertex v,tbrin b0,tbrin &b1);

bool CheckVertexBelongFace(TopologicalGraph &G,tvertex v,tbrin b0,tbrin &b1)
// Checks wether the vertex v belongs to the face defined by b0
// G connected graph
// b1 iseither 0, or a tbrin incident to v and belonging to the face
  {b1 = 0; 
  tbrin b = b0;
  do
      {if(G.vin[-b] == v){b1 = -b;return true;}
      }while((b = G.acir[-b]) != b0);
  return false;
  }

bool CheckBipolarlyOriented(TopologicalGraph &G,tvertex &s,tvertex &t,bool &stConnected,tbrin &bs,bool addEdge)
  {
  // count  the number of sources and sinks
  int ns,nt;
  G.CheckAcyclic(ns,nt);
  if(ns != 1 || nt != 1)return false; //the grap is not bipolarly oriented
  
  s = t = 0;
  //  Find s (vertex with only outgoing edges)   
  bool found = true;;
  tbrin b,b0;
  tvertex v;
  for(v = 1; v <= G.nv();v++)
      {b = b0 =  G.pbrin[v];
      found = true;
      do
          {if(b  < 0){found = false;break;}
          b = G.cir[b];
          }while(b != b0);
      if(found){s = v;break;}
      }
  if(!found)return false;

  //  Find t (vertex with only incoming  edges)  
  for(v = 1; v <= G.nv();v++)
      {b = b0 =  G.pbrin[v];
      found = true;
      do
          {if(b  > 0){found = false;break;}
          b = G.cir[b];
          }while(b != b0);
       if(found){t = v;break;}
      }
  stConnected = false;
  // check that s and  are connected     
  bs = 0;
  b = b0 =  G.pbrin[s];
  do
      {if(G.vin[-b] == t){bs = b;stConnected = true;return true;}
      b = G.cir[b];
      }while(b != b0);

  // check that s and t belong to a same face
  bs = b0 =  G.pbrin[s];
  tbrin bt;
  do
      {// check face defined by b
      if(CheckVertexBelongFace(G,t,bs,bt))break;
      }while((bs =  G.cir[bs])  != b0);
  if(bt == 0){Tprintf("s and t DO NOT belong to a same face"); return false;}

  // Add an edge between s and t
  if(addEdge)
      {G.NewEdge(bs,bt);
      if(G.ComputeGenus() != 0)setPigaleError(-1);
      bs = (tbrin)G.ne(); 
      }
  return true;
  }
int EmbedVision(TopologicalGraph &G)
  {int morg = G.ne();
  if(!G.CheckConnected())G.MakeConnected();
  if(!G.FindPlanarMap())
      {Tprintf("Not Planar Graph");
      for(tedge e = G.ne(); e > morg; e--) G.DeleteEdge(e);
      return -1;
      }
  if(!G.CheckBiconnected())G.Biconnect();
  bool alreadyBipolarOriented = false;
  bool stConnected = false; 
   tvertex s,t;
   tbrin bs,bt;
   bool already2Connected = (morg == G.ne());
   if(already2Connected && (alreadyBipolarOriented = CheckBipolarlyOriented(G,s,t,stConnected,bs,true)) == true)
      {if(stConnected)Tprintf("Using original orientation (s,t are connected)");
      else Tprintf("Using original orientation (s,t are not connected)");
      bt = -bs;
      }
  else
      {// Find reasonable s t vertices
      tedge e;
      tbrin b;
      int len;
      G.LongestFace(bs,len);
      s = G.vin[bs];
      bt = bs;
      for(int i = 1 ;i <= len/2; i++)bt = G.cir[-bt];
      t = G.vin[bt];
      // Check if s an t are connected
      b = bs;
      do
          {if(G.vin[-b] == t){stConnected = true;break;}
          }while((b = G.cir[b]) != bs);
      if(!stConnected)
          {G.NewEdge(bs,bt);
          bs = (tbrin)G.ne();
          }
      s = G.vin[bs];   t = G.vin[-bs];
      // BipolarOrient the graph
      G.BipolarPlan(bs);
      G.FixOrientation();
      }
  
  int n = G.nv();  int m = G.ne();
  // if bs has been reoriented as the packing  suppose that vin[bst]= source
  tbrin bst = (G.vin[bs] != s) ? -bs : bs;
  
  // Compute y coords
  Prop<int> y(G.Set(tvertex()),PROP_DRAW_INT_5); y.clear();
  MaxPath *MP=new MaxPath(n,m);
  for(tedge e = 1; e <= m; e++)
      MP->insert(G.vin[e.firsttbrin()](),G.vin[e.secondtbrin()](),1);
  
  MP->solve(y);
  delete MP;
  int maxyval = y[t];
  
  // compute MaxPath for edges
  svector<int> x(0,m); x.clear();
  MP=new MaxPath(m,2*m);
  svector<tbrin> &Fpbrin = G.ComputeFpbrin();
  tbrin b0,b;
  // out == positif
 
  for(int i = 1; i <= Fpbrin.n(); i++)
      {b0 = Fpbrin[i];
      if(b0.out())
          while((b=-G.acir[b0]).out())
              b0=b;
      else
          do
              {b0=G.cir[-b0];}
          while(b0.in());
      // b0 is the lowest tbrin on the left of the face
      if(b0 == G.cir[bst])continue; // face exterieure
      
      // référence : e
      tedge e = (G.acir[b0]).GetEdge();
      b=b0;
      while (b.out())
          {if(stConnected || b != bst) 
              MP->insert(b.GetEdge()(),e(),1);
          b=G.cir[-b];
          }
      while (b.GetEdge()!=e)
          {MP->insert(e(),b.GetEdge()(),0);
          b=G.cir[-b];
          }
      }


  MP->solve(x);
  delete &Fpbrin;
  delete MP; 

  // computes extremities of vertices
  Prop<int> x1(G.Set(tvertex()),PROP_DRAW_INT_1);
  Prop<int> x2(G.Set(tvertex()),PROP_DRAW_INT_2);
  int maxxval=ComputeExtremities(G,x,x1,x2,morg);

  Prop1<int> m_org(G.Set(),PROP_TMP);
  m_org() = morg;
  for(tedge e = G.ne(); e > morg; e--)
      G.DeleteEdge(e);

  Prop1<int> maxx(G.Set(),PROP_DRAW_INT_1);
  Prop1<int> maxy(G.Set(),PROP_DRAW_INT_2);
  maxx()=maxxval;  maxy()=maxyval;
  Prop1<Tpoint> pmin(G.Set(),PROP_POINT_MIN);
  Prop1<Tpoint> pmax(G.Set(),PROP_POINT_MAX);
  pmin() = Tpoint(-1,-1);
  pmax() = Tpoint(maxxval+1,maxyval+1);


  Prop<Tpoint> P1(G.Set(tedge()),PROP_DRAW_POINT_1);
  Prop<Tpoint> P2(G.Set(tedge()),PROP_DRAW_POINT_2);
  for (tedge e=1; e<= G.ne(); e++)
      {P1[e]=Tpoint(x[e],y[G.vin[e.firsttbrin()]]);
      P2[e]=Tpoint(x[e],y[G.vin[e.secondtbrin()]]);
      }
  
  return 0;
  }

int ComputeExtremities(TopologicalGraph &G,svector<int> &x, 
                       svector<int> &x1,svector<int> &x2,int morg)
  {tvertex v;
  tedge e;
  int n=G.nv();
  int m=G.ne();

  int maxxval=0;
  for(e=1;e <=m;e++)
      if(x[e] > maxxval)maxxval = x[e];
  
  x2.clear();
  for (v=1; v <= n;v++)
      x1[v] = maxxval + 1;
  int xe;
  for (e=1;e<=morg;e++)
      {xe=x[e];
      v=G.vin[e.firsttbrin()];
      if (x1[v]>xe) x1[v]=xe;
      if (x2[v]<xe) x2[v]=xe;
      v=G.vin[e.secondtbrin()];
      if (x1[v]>xe) x1[v]=xe;
      if (x2[v]<xe) x2[v]=xe;
      }
  // For isolated vertices
  for (v=1; v <= n;v++)
      if(x1[v] == maxxval + 1)
          {xe = x[G.FirstBrin(v).GetEdge()];
          x1[v] = x2[v] = xe;
          }
  return maxxval;
  }




