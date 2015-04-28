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



#include <TAXI/graph.h>
#include <TAXI/bicon.h>
#include <TAXI/lralgo.h>
#include <TAXI/embed.h>
#include <TAXI/color.h>
#include <TAXI/DFSGraph.h> 
#include <TAXI/ndfs.h>
#include <TAXI/Tmessage.h> 

int NumberOfParallelEdges(int n, int m, const svector<tvertex> &vin)
// after DFS
  {int p=0;
  svector<tedge>link(1,m); link.clear(); link.SetName("Planar:link");
  svector<tedge>top1(1,n); top1.clear(); top1.SetName("Planar:top1");
  svector<tedge>top2(1,n); top2.clear(); top2.SetName("Planar:top2");
  tvertex u,v,w;
  tedge e,next;
  //First sort with respect to biggest label
  for(e = m;e >= 1;e--)
      {v = vin[-e];
      link[e] = top1[v];top1[v] = e;
      }
  //First sort with respect to smallest label
  for(u = n;u > 1;u--)
      {e = top1[u];
      while(e!=0)
          {next = link[e];     //as link is modified
          v = vin[e];
          link[e] = top2[v]; top2[v] = e;
          e = next;
          }
      }
  // Count Multiple edges
  for(v = n-1;v >= 1;v--)
      {e = top2[v];
      while(e!=0)
          {next = link[e];
          u = vin[-e];
          w = vin[-next];
          if(u == w)
              ++p;
          e = next;
          }
      }
  return p;
  }
int TopologicalGraph::Planarity(tbrin b0)
  {if(!ne())return 1;
  if(debug())DebugPrintf("Executing TopologicalGraph::Planarity");
// #ifdef TDEBUG
//   if(!DebugCir())
//       {DebugPrintf("input Cir is wrong");setPigaleError(A_ERRORS_BAD_INPUT);return A_ERRORS_BAD_INPUT;}
// #endif
  int m_origin = ne();
  MakeConnected();
  int m = ne();
  int n = nv();
#ifdef TDEBUG
  if(debug())DebugPrintf("n=%d m=%d",n,m);
#endif
  svector<tvertex> nvin(-m,m); nvin.SetName("Planarity:nvin");
  svector<tvertex> &low = *new svector<tvertex>(0,n);
  svector<tbrin> xcir; xcir.SetName("Planarity:xcir");
  xcir = cir;
  xcir[0] = b0; xcir[acir[b0]] = 0;
  if(!GDFSRenum(xcir,nvin)) // Error
      {delete &low;
      DebugPrintf("GDFSRenum ERROR");
      setPigaleError(A_ERRORS_GDFSRENUM,"GDFSRenum ERROR");return A_ERRORS_GDFSRENUM;
      }
  nvin.Tswap(vin);
  _Bicon Bicon(n); 
  int ret = bicon(n,m,vin,Bicon,low);
  if(ret) Prop1<int> isbicon(Set(),PROP_BICONNECTED);
  _LrSort LrSort(n,m);
  LralgoSort(n,m,vin,Bicon,low,LrSort);
  delete &low;

  _Hist Hist(n,m);
  ret = lralgo(n,m,vin,Bicon,LrSort,Hist);
  if(debug())DebugPrintf("Executing Embed lralgo = %d (1-Planar 0-NonPlanar)",ret);
  Embed Embedder(me(),Bicon,LrSort,Hist);
  Embedder();

  nvin.Tswap(vin);
  Prop<int> ivl(PV(),PROP_LABEL);
  Prop<int> iel(PE(),PROP_LABEL);
  //Prop<tbrin> iel(PE(),PROP_LABEL);
  tbrin b;
  
  for (b = 1; b <= ne(); b++)
      { tbrin b2 = cir[b];
      if (b2<0) xcir[iel[b]] = -iel[-b2];
      else xcir[iel[b]] = iel[b2];
      b2 = cir[-b];
      if (b2<0) xcir[-iel[b]] = -iel[-b2];
      else xcir[-iel[b]] = iel[b2];
      }
    
  xcir[0]=0;
  // the lralgo cir is clockwise
  xcir.Tswap(cir);
  // xcir.Tswap(acir);
  for (b = -ne(); b<= ne(); b++)
      acir[cir[b]] = b;
  //  cir[acir[b]] = b;
  if(extbrin() > 0)
      extbrin() = iel[extbrin()];
  else
      extbrin() = -iel[-extbrin()];
  b = iel[1];
  pbrin[vin[b]] = b;
  for(tvertex v = 2;v <= nv();v++)
      {b = (tbrin)-iel[v-1];
      pbrin[vin[b]] = b;
      }

#ifdef TDEBUG    
  if(ret)
      {int g;
      if((g = ComputeGenus()) != 0)
          {DebugPrintf("ERROR genus:%d",g);setPigaleError(A_ERRORS_PLANARITY);}
      }
#endif
  if(m != m_origin)
      for(tedge e = m; e() > m_origin;--e) DeleteEdge(e);

  Prop1<int> maptype(Set(),PROP_MAPTYPE);
  maptype() = PROP_MAPTYPE_LRALGO;
  if(ret)
      {Prop1<int> isplanar(Set(),PROP_PLANAR);
      planarMap() = 1;
      }
  else  planarMap() = -1;
  if(debug())DebugPrintf("    END Planarity");
  return ret;
  }
int TopologicalGraph::TestPlanar()
  {int m = ne();
  int n = nv();
  int m_origin = m;
  if(m < 9 || n < 4){Prop1<int> isplanar(Set(),PROP_PLANAR); return 1;}
  if(debug())DebugPrintf("Executing TopologicalGraph:TestPlanar");
// #ifdef TDEBUG
//   if(!DebugCir())
//       {DebugPrintf("input Cir is wrong");setPigaleError(A_ERRORS_BAD_INPUT);return A_ERRORS_BAD_INPUT;}
// #endif
  svector<tvertex> nvin(-m,m); nvin.SetName("TestPlanar:nvin");
  // DFS calls GDFS after some initializations

  if(!DFS(nvin)) //Not connected graph
      {MakeConnected();
      m = ne();
      nvin.resize(-m,m);
      DFS(nvin);
      for(tedge e = m; e() > m_origin;--e)
          DeleteEdge(e);
      }
  else
      Prop1<int>is_connected(Set(),PROP_CONNECTED);

  if(m - n < 3)
      {Prop1<int> isplanar(Set(),PROP_PLANAR);
      return 1;
      }
  if(Set().exist(PROP_SIMPLE) && m > 3*n - 6)return 0;
  svector<tvertex> low(0,n);low.SetName("TestPlanar:low");
  _Bicon Bicon(n);
  if(bicon(n,m,nvin,Bicon,low))
      Prop1<int> isbicon(Set(),PROP_BICONNECTED);
  _LrSort LrSort(n,m);
  LralgoSort(n,m,nvin,Bicon,low,LrSort);
  _FastHist Hist(m);
  int ret = fastlralgo(n,m,nvin,Bicon,LrSort,Hist);
  if(ret)Prop1<int> isplanar(Set(),PROP_PLANAR);
  else planarMap() = -1;
  if(debug())DebugPrintf("    END TopologicalGraph:TestPlanar");
  return ret;
  }
int TopologicalGraph::NewPlanarity(tbrin b0)
  {if(!ne())return 1;
  if(debug())DebugPrintf("Executing TopologicalGraph::NewPlanarity");
  
  int m_origin = ne();
  MakeConnected();
 /* 
 #ifdef TDEBUG
   if(!DebugCir()){DebugPrintf("input Cir is wrong");setPigaleError(A_ERRORS_BAD_INPUT);return A_ERRORS_BAD_INPUT;}
 #endif
 */
  
  int m = ne();
  int n = nv();
#ifdef TDEBUG
  if(debug())DebugPrintf("n=%d m=%d",n,m);
#endif
  svector<tvertex> nvin(-m,m);  nvin.SetName("NewPlanarity:nvin");
  svector<tvertex> &low = *new svector<tvertex>(0,n);
  svector<tbrin> xcir;          xcir.SetName("NewPlanarity:xcir");
  xcir = cir;
  xcir[0] = b0; xcir[acir[b0]] = 0;
  if(!GDFSRenum(xcir,nvin)) // Error
      {delete &low;
      DebugPrintf("GDFSRenum ERROR");
      setPigaleError(A_ERRORS_GDFSRENUM,"GDFSRenum ERROR");return A_ERRORS_GDFSRENUM;
      }
  nvin.Tswap(vin);
  _Bicon Bicon(n);
  int ret = bicon(n,m,vin,Bicon,low);
  if(ret) Prop1<int> isbicon(Set(),PROP_BICONNECTED);
  _LrSort LrSort(n,m);
  NewLralgoSort(n,m,vin,Bicon,low,LrSort);
  delete &low;

  _Hist Hist(n,m);
  ret = Newlralgo(n,m,vin,Bicon,LrSort,Hist);
  if(debug())DebugPrintf("    New Embed lralgo = %d (1-Planar 0-NonPlanar)",ret);
  Embed Embedder(me(),Bicon,LrSort,Hist);
  Embedder();
  nvin.Tswap(vin);
  Prop<int> ivl(PV(),PROP_LABEL);
  Prop<int> iel(PE(),PROP_LABEL);
  //Prop<tbrin> iel(PE(),PROP_LABEL);
  tbrin b;
  for (b = 1; b <= ne(); b++)
      { tbrin b2 = cir[b];
      if (b2<0) xcir[iel[b]] = -iel[-b2];
      else xcir[iel[b]] = iel[b2];
      b2 = cir[-b];
      if (b2<0) xcir[-iel[b]] = -iel[-b2];
      else xcir[-iel[b]] = iel[b2];
      }
  xcir[0]=0;
  // the lralgo cir is clockwise
  xcir.Tswap(cir);
  // xcir.Tswap(acir);
  for (b = -ne(); b<= ne(); b++)
      acir[cir[b]] = b;
  //  cir[acir[b]] = b;
  if(extbrin() > 0)
      extbrin() = iel[extbrin()];
  else
      extbrin() = -iel[-extbrin()];
  b = iel[1];
  pbrin[vin[b]] = b;
  for(tvertex v = 2;v <= nv();v++)
      {b = (tbrin)-iel[v-1];
      pbrin[vin[b]] = b;
      }

#ifdef TDEBUG    
  if(ret)
      {int g;
      if((g = ComputeGenus()) != 0)
          {DebugPrintf("ERROR genus:%d",g);setPigaleError(A_ERRORS_PLANARITY);}
      }
#endif
  if(m != m_origin)
      for(tedge e = m; e() > m_origin;--e) DeleteEdge(e);

  Prop1<int> maptype(Set(),PROP_MAPTYPE);
  maptype() = PROP_MAPTYPE_LRALGO;
  if(ret)
      {Prop1<int> isplanar(Set(),PROP_PLANAR);
      planarMap() = 1;
      }
  else  planarMap() = -1;
  if(debug())DebugPrintf("    END New Planarity");
  return ret;
  }
int TopologicalGraph::TestNewPlanar()
  {int m = ne();
  int n = nv();
  int m_origin = m;
  if(m < 9 || n < 4){Prop1<int> isplanar(Set(),PROP_PLANAR); return 1;}
// #ifdef TDEBUG
//   if(!DebugCir())
//       {DebugPrintf("input Cir is wrong");setPigaleError(A_ERRORS_BAD_INPUT);return A_ERRORS_BAD_INPUT;}
// #endif
  if(debug())DebugPrintf("Executing TopologicalGraph:NewTestPlanar");
  svector<tvertex> nvin(-m,m);nvin.SetName("TestNewPlanar:nvin");
  // DFS calls GDFS after some initializations
  if(!DFS(nvin)) //Not connected graph
      {MakeConnected();
      m = ne();
      nvin.resize(-m,m);
      DFS(nvin);
      for(tedge e = m; e() > m_origin;--e)
          DeleteEdge(e);
      }
  else
      Prop1<int>is_connected(Set(),PROP_CONNECTED);

  if(m - n < 3)
      {Prop1<int> isplanar(Set(),PROP_PLANAR);
      return 1;
      }
  if(Set().exist(PROP_SIMPLE) && m > 3*n - 6)return 0;
  svector<tvertex> low(0,n);low.SetName("low TestPlanar");
  _Bicon Bicon(n);
  if(bicon(n,m,nvin,Bicon,low))
      Prop1<int> isbicon(Set(),PROP_BICONNECTED);
  _LrSort LrSort(n,m);
  NewLralgoSort(n,m,nvin,Bicon,low,LrSort);
  int ret = Newfastlralgo(n,m,nvin,Bicon,LrSort);
  if(ret)Prop1<int> isplanar(Set(),PROP_PLANAR);
  else planarMap() = -1;
  if(debug())DebugPrintf("    END TopologicalGraph:NewTestPlanar");
  return ret;
  }
int TopologicalGraph::TestPlanar2()
// uses the class DFSGraph
  {if(debug())DebugPrintf("Executing TopologicalGraph:TestPlanar2");
  GraphContainer DFSContainer;
  int m = ne();
  int n = nv();
  int m_origin = m;
  if(m < 9 || n < 4) return 1;
  DFSGraph DG(DFSContainer,*this);

  if (!Set().exist(PROP_CONNECTED))
      {MakeConnected();
      m = ne();
      DG.setsize(tedge(),m);
      DG.DoDFS();
      for(tedge e = m; e() > m_origin;--e)
          DeleteEdge(e);
      }

  if(m - n < 3) return 1;

  if(Set().exist(PROP_SIMPLE) && m > 3*n - 6)
      return  0;
  int ret = DG.TestPlanar();
  if(ret)Prop1<int> isplanar(Set(),PROP_PLANAR);
  else planarMap() = -1;
  if(debug())DebugPrintf("    END:TestPlanar2 -> %d (1-planar)",ret);
  return ret;
  }
int TopologicalGraph::RemoveIsthmus()
  {GraphContainer DFSContainer;
  MakeConnected();
  DFSGraph DG(DFSContainer,*this);
  DG.bicon();
  Prop<int> status(DG.Set(tvertex()),PROP_TSTATUS);
  svector<bool> toerased(1,ne()); toerased.clear();
  int n = 0;
  for(int i = nv() -1;i > 0;i--)
      if(status[i] == PROP_TSTATUS_ISTHMUS)
	{++n;toerased[DG.ie(i)] = true;}
  for(tedge e = ne(); e > 0;e--)
      if(toerased[e])DeleteEdge(e);
  RemoveIsolatedVertices();
  return n;
  }
int TopologicalGraph::CotreeCritical(int &ret)
  {if(debug())DebugPrintf("Executing CotreeCritical");
  Simplify();
  init();
  int m_origin = ne();
  MakeConnected();Set().erase(PROP_CONNECTED);
  GraphContainer DFSContainer;
  DFSGraph DG(DFSContainer,*this);
  ret = DG.CotreeCritical();
  Prop<int> ewidth(Set(tedge()),PROP_WIDTH);
  Prop<short> ecolor(Set(tedge()),PROP_COLOR);
  tedge e;
  for(e = 1;e <= ne();e++) {ewidth[e]=1;ecolor[e]= Grey1;}
  for(e = 1;e <= DG.ne();e++)
      {ewidth[DG.ie(e)]=2;ecolor[DG.ie(e)]= Black;}
  for(e = ne(); e > m_origin;e--)
      DeleteEdge(e);
  return 0;
  }
int TopologicalGraph::CotreeCritical(svector<bool> &mark)
  {if(debug())DebugPrintf("Executing CotreeCritical(mark)");
  Simplify();
  MakeConnected();Set().erase(PROP_CONNECTED);
  GraphContainer DFSContainer;
  DFSGraph DG(DFSContainer,*this);
  DG.CotreeCritical();
  mark.clear();
  for(tedge e = 1;e <= DG.ne();e++)
      mark[DG.ie(e)] = true;
  return 0;
  }
int TopologicalGraph::Kuratowski(svector<bool> &mark)
  {if(debug())DebugPrintf("Executing Kuratowski(mark)");
  Simplify();
  MakeConnected();
  GraphContainer DFSContainer;
  DFSGraph DG(DFSContainer,*this);
  int ret = DG.CotreeCritical();
  if(!ret) return 0;  // Planar graph
  if((ret = DG.Kuratowski()) != 0)return ret;
   
  // transfert de DG sur GG
  Prop<bool> keep(DG.Set(tedge()),PROP_MARK);
  mark.clear();
  for(tedge e = 1;e <= DG.ne();e++)
      if(keep[e])mark[DG.ie(e)] = true;

  return 0;
  }
int TopologicalGraph::Kuratowski()
// Kuratowski which color the edges of the configuration in Black
  {if(debug())DebugPrintf("Executing Kuratowski");
  Simplify();
  int m_origin = ne();
  MakeConnected();
  GraphContainer DFSContainer;
  DFSGraph DG(DFSContainer,*this);
  tedge e;
  tvertex v;
  int ret = DG.CotreeCritical();
  if(!ret) return 0;  // Planar graph
  if((ret = DG.Kuratowski()) != 0)return ret;
  Prop<int> ewidth(Set(tedge()),PROP_WIDTH);
  Prop<short> ecolor(Set(tedge()),PROP_COLOR);
  Prop<short> vcolor(Set(tvertex()),PROP_COLOR);

   
  // transfert de DG sur GG
  Prop<bool> keep(DG.Set(tedge()),PROP_MARK);
  for(e = 1;e <= ne();e++) {ewidth[e]=1;ecolor[e]= Grey1;}
  for(e = 1;e <= DG.ne();e++)
      if(keep[e]){ewidth[DG.ie(e)]=2;ecolor[DG.ie(e)]= Black;}
  for(e = ne(); e > m_origin;e--)DeleteEdge(e);

  return 0;
  }
int TopologicalGraph::KKuratowski()
// Kuratowski which erases edges not belonging to the configuration
  {if(debug())DebugPrintf("Executing KKuratowski");
  Simplify();
  int m_origin = ne();
  MakeConnected();Set().erase(PROP_CONNECTED);

  GraphContainer DFSContainer;
  DFSGraph DG(DFSContainer,*this);
  tedge e;
  int ret = DG.CotreeCritical();
  if(!ret) return 0;  // Planar graph
  if((ret = DG.Kuratowski()) != 0)
      return ret;
  tvertex v;
  Prop<int> ewidth(Set(tedge()),PROP_WIDTH);
  Prop<short> ecolor(Set(tedge()),PROP_COLOR);
  // transfert de DG sur GG
  Prop<bool> keep(DG.Set(tedge()),PROP_MARK);
  for(e = 1;e <= ne();e++) {ewidth[e]=1;ecolor[e]= Grey1;}
  for(e = 1;e <= DG.ne();e++)
      if(keep[e]){ewidth[DG.ie(e)]=2;ecolor[DG.ie(e)]= Black;}
  // Test success
  GraphContainer GC(Container());
  GeometricGraph GG(GC);
  for(e = GG.ne();e>=1;e--)
      if(GG.ewidth[e] == 1)GG.DeleteEdge(e);
  if(GG.TestPlanar())return -2;
  for(e = ne(); e > m_origin;e--)DeleteEdge(e);
  return 0;
  }
int TestOuterPlanar(TopologicalGraph &G)
  {if(G.ne() < 6)return true;
  if(debug())DebugPrintf("Executing OuterPlanar");
  bool simple = G.Set().exist(PROP_SIMPLE);
  bool bipartite = G.Set().exist(PROP_BIPARTITE);
  tvertex v0 = G.NewVertex();
  for(tvertex v = 1; v < v0;v++)
      G.NewEdge(v,v0);
  int OuterPlanar = G.TestPlanar();
  G.DeleteVertex(v0);
  if(debug())DebugPrintf("    END OuterPlanar");
  if(simple)Prop1<int> simple(G.Set(),PROP_SIMPLE);
  if(bipartite)Prop1<int> bipartite(G.Set(),PROP_BIPARTITE);
  return OuterPlanar;
  }
int FindOuterPlanar(TopologicalGraph &G, int depth)
 {
 Prop<short> ecolor(G.Set(tedge()),PROP_COLOR);
 tedge e;
 int m=G.ne();
 int ok;
 // for (e=1; e<=m; e++) ecolor[e]=Black;
 tvertex v0,v1;
 if (TestOuterPlanar(G)) return 1; // depth=0
 if (depth==0) return 0;
 for (e=m; e>0; --e)
     {v0=G.vin[e.firsttbrin()];
     v1=G.vin[e.secondtbrin()];
     G.DeleteEdge(e);
     ok=FindOuterPlanar(G,depth-1);
     G.NewEdge(v0,v1);
     if (ok) 
	 {ecolor[m]=Red;
	 return 1;
	 }
     else
	 {ecolor[m]=Black;}
     }
 return 0;
 }


int FindOuterPlanar(TopologicalGraph &G)
  {Prop<short> ecolor(G.Set(tedge()),PROP_COLOR);
  Prop<int> ewidth(G.Set(tedge()),PROP_WIDTH);
  int m=G.ne();
  tedge e;
  for (e=1; e<=m; e++) {ecolor[e] = Black;ewidth[e] = 2;}
  if(TestOuterPlanar(G))return 0;
  tvertex v0,v1;
  int outerplanar;
  for(e = m; e >= 1;e--)
      {v0 = G.vin[e]; v1 = G.vin[-e];
      G.DeleteEdge(e);
      outerplanar = TestOuterPlanar(G);
      G.NewEdge(v0,v1);
      if(outerplanar) {ecolor[m] = Red;return 1;}
      }
  for (e=1; e<=m; e++)ecolor[e] = Red;
  return -1;
  }
