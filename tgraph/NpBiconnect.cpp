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
#include <TAXI/color.h>

int TopologicalGraph::NpBiconnectVertex()
  {return NpBiconnect(true);
  }
int TopologicalGraph::NpBiconnect(bool withVertices)
  {if(Set().exist(PROP_BICONNECTED))return 0;
  if(withVertices)MakeConnectedVertex();
  else MakeConnected();

  if(CheckPlanar() && !withVertices)return Biconnect();
  if(debug())DebugPrintf("Executing NpBionnected withVertices=%d",withVertices);
  // DFS which computes the articulated edges
  // At the root there is always an articulated edge :
  // 1 an isthmus    -1 a tree edge
  int n = nv();
  int m = ne();

  // First and Second will contain the k pair of vertices to join
  int k = 0;
  svector<tvertex> First(n); First.SetName("First");
  svector<tvertex> Second(n);Second.SetName("Second");
  
  svector<int> low(1,n);low.SetName("low");
  svector<int> dfsnum(1,n); dfsnum.SetName("dfsnum");
  svector<tbrin> InTreeBrin(0,n);
  InTreeBrin.clear(); InTreeBrin.SetName("InTreeNrin");
  svector<int> articu(1,m); articu.clear(); articu.SetName("articu");

  tbrin b = 1;
  tvertex w,v = vin[b];
  tvertex root = v;
  low[v] = dfsnum[v] = 1;
  InTreeBrin[v] = b;
  int y = 1;
  int mm =0;
  
  do
      {w = vin[-b];
      if(InTreeBrin[w] != 0) 
          {if(b == InTreeBrin[v])    // Backtacking from v to w 
              {b.cross();
              low[dfsnum[w]] = Min(low[dfsnum[v]],low[dfsnum[w]]);
              if(low[dfsnum[v]] >= dfsnum[w])
                  {if(w != root) 
                      articu[b.GetEdge()] = 1;
                  else if(low[dfsnum[v]] > dfsnum[w] )
                      articu[b.GetEdge()] = 1;
                  else
                      articu[b.GetEdge()] = -1;
                  }
              ++mm;
              v = w;
              }
          else if (dfsnum[v] > dfsnum[w]) // high cotree edge
              low[dfsnum[v]] = Min(low[dfsnum[v]],dfsnum[w]);
          else ++mm;
          if(mm == m)break;
          }
      else // Climbing in the tree from v to w 
          {b.cross();
          InTreeBrin[w] = b;
          dfsnum[w] = ++y;
          low[y] = y;
          v = w;
          }
      b = cir[b];
      }while(1);
  
  // Second DFS adding edges
  InTreeBrin.clear();InTreeBrin[v] = 1;
  mm = 0; b = 1; v = vin[b];
  tvertex v2,vv,v1 = 0;
  tbrin bb;
  do
      {w = vin[-b];
      if(InTreeBrin[w] != 0) 
          {if(b == InTreeBrin[v])    // Backtacking from v to w 
              {b.cross();
              if(articu[b.GetEdge()] == 1 && w != root)
                  {v2 = v;
                  if(!v1) // we did not find an isthmus before
                      v1 = vin[-InTreeBrin[w]];
                  else if(v1 != root)
                      v1 = vin[-InTreeBrin[v1]];
                  First[++k] = v2;
                  Second[k]  = v1;
                  // Declare non articulated edges between v1 and v2
                  bb = b;vv = w;
                  do
                      {articu[bb.GetEdge()] = -1;
                      bb = InTreeBrin[vv];
                      vv = vin[-bb];
                      }while(vv != v1);
                  v1 = 0;
                  }
              ++mm;
              v = w;
              }
          else if (dfsnum[v] < dfsnum[w]) // low cotree edge
              ++mm;
          if(mm == m)break;
          }
      else // Climbing in the tree from v to w 
          {b.cross();
          InTreeBrin[w] = b;
          if(!v1 && articu[b.GetEdge()] == 1)v1 = v;
          v = w;
          }
      b = cir[b];
      }while(1);
  // Insert edges around the root
   b = 1;
   v1 = vin[-b];
   while((b = cir[b]) != 1)
       {if(articu[b.GetEdge()])
           {v2 = vin[-b];
           First[++k] = v2;
           Second[k]  = v1;
           v1 = v2;
           }
       }
   tedge m0 = ne();
   // Do Insert Edges
   for(int i = 1;i <= k;i++)
       NewEdge(First[i],Second[i]);
   if(withVertices) // bissect all added edges 
       { tedge m1 = ne();
       if(Set(tvertex()).exist(PROP_COORD)) // Geometric Graph
           {Prop<Tpoint> vcoord(Set(tvertex()),PROP_COORD);
           for(tedge e  = m0+1; e <= m1;e++ )
               {tvertex v1 = vin[e];tvertex v2 = vin[-e];
               tvertex v =  BissectEdge(e);
               vcoord[v] = (vcoord[v1]  + vcoord[v2])/2.;
               }
           }
       else
            for(tedge e  = m0+1; e <= m1;e++ )
                BissectEdge(e);
       }
   return 0;
  }

