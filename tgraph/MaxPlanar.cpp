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
#include <TAXI/color.h>
#include <TAXI/DFSGraph.h>

//FAST ALGO *******************************************************************
tedge EraseMostMarkedEdge(TopologicalGraph& G0,svector<int>& count) //fast algo
  {Prop<tedge> initedge(G0.Set(tedge()),PROP_INITIALE);
  GraphContainer DFSContainer;
  DFSGraph DG(DFSContainer,G0);
  int ret = DG.CotreeCritical();
  if(!ret)return 0;  // Bug: CotreeCritical graph not found
  DG.Kuratowski();
  Prop<bool> keep(DG.Set(tedge()),PROP_MARK); // Only valid if kuratowski
  int maxcount = 0;
  tedge f,f0;
  tedge e,e0 = 0;
  
  for(tedge j = DG.ne();j > 0;j--)
      {if(!keep[j])continue; 
      // j belongs to the Kuratowski
      f0 = DG.ie(j);   // f0 belongs to G0  
      f = initedge[f0];
      if(++count[f] > maxcount) 
          {maxcount = count[f];
          e0 = f0;
          }
      }      
 
  e = initedge[e0];  // e has the biggest count
  if(e0())G0.DeleteEdge(e0);
  else 
      setPigaleError(-1,"EraseMostMarkedEdge");   

  return e;
  }
void ReinsertSomeEdges(TopologicalGraph& G,TopologicalGraph& G0
                       ,svector<tedge>& tab,int& n)
// G0 is planar and we try to add edges, keeping it planar                       
  {tedge e,e0;
  int nn = n;
  int i,j;
  for(i = 1,j = 1;i <= nn;i++)
      {e = tab[j] = tab[i];
      e0 = G0.NewEdge(G.vin[e],G.vin[-e]);
      if(G0.TestPlanar() == 1)
          n--;
      else
          {j++;
          G0.DeleteEdge(e0);// as G0 would be non planar
          }
      }
  }
int FindNPSet(TopologicalGraph &G, svector<tedge>&tab) // fast algo
  {
  svector<int> count(0,G.ne()); count.clear();count.SetName("Maxplanar:count");
  // G0 is a copy on which we work
  GraphContainer GC(G.Container());
  TopologicalGraph G0(GC);
  Prop<tedge> initedge(G0.Set(tedge()),PROP_INITIALE);initedge.SetName("initedge");
  tedge e; 
  for(e = 0; e <= G0.ne(); e++)initedge[e]=e;
  int n = 0;
  while (G0.TestPlanar() !=1)
      {if((e = EraseMostMarkedEdge(G0,count)) == 0)return 0;
      tab[++n]= e;
      }
  ReinsertSomeEdges(G,G0,tab,n);
  //if(!e())cout<<"EraseMostMarkedEdge ERROR"<<endl;
  return n; // number of edges to delete
  }
int FindNPSet(TopologicalGraph &G) //fast algo
  {if(debug())DebugPrintf("Executing FindNPSet");
  G.init();
  svector<tedge> tab(1,G.ne()); tab.SetName("FindNPSet:tab");
  G.Simplify();
  int m_origin = G.ne();
  G.MakeConnected();
  
  int n0 = FindNPSet(G,tab);
  if(!n0)return 0;
  Prop<short> ecolor(G.Set(tedge()),PROP_COLOR);
  Prop<int> ewidth(G.Set(tedge()),PROP_WIDTH);
  tedge e;
  for(e = 1; e <= G.ne();e++)
      {ewidth[e] = 1; ecolor[e] = Black;}
  for (int i=1; i<=n0; i++)
      {e=tab[i]; ewidth[e] = 2; ecolor[e] = Blue;
      }
  for(e = G.ne(); e > m_origin;e--) G.DeleteEdge(e);
  if(debug())DebugPrintf("    END Executing FindNPSet");
  return n0;
  }
//SLOW ALGO ******************************************************  
  
tedge EraseMostMarkedEdge(TopologicalGraph& G0,svector<bool> &mark,
                          svector<int>& count)
  {tedge e,e0,f0,f;
  Prop<tedge> initedge(G0.Set(tedge()),PROP_INITIALE);
  GraphContainer DFSContainer;
  DFSGraph DG(DFSContainer,G0);
  DG.CotreeCritical();// was commented
  DG.Kuratowski();
  Prop<bool> keep(DG.Set(tedge()),PROP_MARK); 
  int maxcount=0;
  e0 = e = 0;
    for(tedge j = DG.ne();j > 0;j--)
      {if(!keep[j])continue;
      f0 = DG.ie(j);   // f0 belongs to G0  
      f = initedge[f0]; // f belongs to G
      ++count[f];
      if (count[f]>maxcount || (count[f]==maxcount && !mark[f])) 
          {maxcount = count[f];
          e0 = f0;
          e = f;
          }
      } 
 /* 
  for(tedge j = DG.ne();j > 0;j--)
      {if(!keep[j])continue;
      f0 = DG.ie(j);   // f0 belongs to G0  
      f = initedge[f0]; // f belongs to G
      ++count[f0];
      if (count[f0]>maxcount || count[f0]==maxcount && !mark[f]) 
          {maxcount = count[f0];
          e0 = f0;
          e = f;
          }
      } 
 */
  if (e0!=0) G0.DeleteEdge(e0);
  return e;
  }
int FindNPSet(TopologicalGraph &G, svector<bool> &mark, svector<tedge>&tab)
  {tedge e; 
  int n=0;
  svector<int> count(0,G.ne()); count.clear();count.SetName("FindNPSet:count");
  GraphContainer GC(G.Container());
  TopologicalGraph G0(GC);
  Prop<tedge> initedge(G0.Set(tedge()),PROP_INITIALE);initedge.SetName("FindNPSet:initedge");
  for (e=0; e<=G0.ne(); e++) initedge[e]=e;
 
  while (G0.TestPlanar() !=1)
      {if ((e=EraseMostMarkedEdge(G0,mark,count))==tedge(0)) return 0;
      tab[++n]=e;
      }
  ReinsertSomeEdges(G,G0,tab,n);
  bool newones = false;
  for (int i=1; i<=n; i++) 
      if (!mark[tab[i]])
          {mark[tab[i]]=true;
          newones=true;
          }
  return newones ? n : -n;
  }

int TopologicalGraph::MaxPlanar(svector<bool>  &keep)
  {// Precondition the graph is simple
  if(!CheckSimple())return -1;
  svector<tedge>  ToBeErased(1,ne());ToBeErased.SetName("MxPlanar:ToBeErased");
  svector<bool>  mark(1,ne()); mark.clear(); mark.SetName("MxPlanar:mark");
  svector<tedge> tab(1,ne()); tab.SetName("MxPlanar:tab");
  tedge e;
  int n,i;
  int n0 = ne();
  // Find a first solution
  while((n=FindNPSet(*this,mark,tab))!=0)
      {
      if (n<0)
          {n=-n;
          if (n>=n0) break;
          }
      if(n < n0)
          {for(i = 1;i <= n; i++)ToBeErased[i] = tab[i];
          if((n0 = n) <= 1)break;
          }
      }
  // Optimize the solution
  tedge e0,ee,einit;
  int inserted,j;
  bool ok;
  svector<bool> ReInserted(1,n0); ReInserted.SetName("ReInserted"); 

  for(ee = ne();ee > 0;ee--)
      {GraphContainer GC(Container());
      TopologicalGraph G0(GC);
      Prop<tedge> initedge(G0.Set(tedge()),PROP_INITIALE);
      for(e = 1;e <= ne();e++)initedge[e] = e;
      mark.clear();
      for(i = 1;i <= n0;i++) 
      	{if(!ToBeErased[i])cout<<"Tobe=0 i:"<<i<<" n0:"<<n0<<endl;
      	if(ToBeErased[i] != 0)// ???
      	mark[ToBeErased[i]] = true;
      	}
      G0.DeleteEdge(ee);
      ok = true;
      for(e = G0.ne();e > 0;e--)
          {einit = initedge[e];
          if(!einit)cout<<"einit=0"<<endl;
          if(einit == ee){ok = false;break;}
          if(mark[einit])G0.DeleteEdge(e);
          }
      if(ok == false)continue;    
      // Try to add more than one edge
      ReInserted.clear();
      inserted = 0;
      for(i = 1;i <= n0;i++)
          {e = ToBeErased[i];
          e0 = G0.NewEdge(vin[e],vin[-e]);
          if(G0.TestPlanar() == 1)
              {++inserted;
              ReInserted[i] = true;
              }
          else
              G0.DeleteEdge(e0);
          }
      if(inserted <= 1)continue;
      // Solution can be optimized
      j = 0;
      for(i = 1;i <= n0;i++)
          if(ReInserted[i] == false)ToBeErased[++j] = ToBeErased[i];
      ToBeErased[++j] = ee;
      n0 = j;
      }
  for(ee = 1;ee <= ne();ee++)
      keep[ee] = true;
  for(i = 1;i <= n0;i++)
      keep[ToBeErased[i]] = false;

  return n0;
  }
int TopologicalGraph::MaxPlanar()
  {if(!CheckSimple())return -1;
  int m_origin = ne();
  MakeConnected();
  svector<bool>  keep(1,ne());
  int n0 = MaxPlanar(keep);
  if(n0 < 0){cout<<"  MaxPlanar: n0:"<<n0<<endl;return n0;}
  Prop<short> ecolor(Set(tedge()),PROP_COLOR);
  Prop<int> ewidth(Set(tedge()),PROP_WIDTH);
  tedge e;
  for(e = 1; e <= ne();e++)
      if(keep[e])
          {ewidth[e] = 1; ecolor[e] = Black;}
      else
          {ewidth[e] = 2; ecolor[e] = Blue;}
  for(e = ne(); e > m_origin;e--)DeleteEdge(e);
  return n0;
  }

