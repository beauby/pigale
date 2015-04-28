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

// find an oriented path from a vertex $u$ such that $deg^-(u) < max_indeg-1$
// to the vertex $v_0$.
static int FindOrientedPath(TopologicalGraph &G, tvertex v, int max_indeg, IntList &elist)
  {
  svector<int> mark(1,G.nv(),0);  mark.SetName("mark");
  svector<int> brin(1,G.nv(),0);  brin.SetName("brin");
  tvertex vopp;
  tbrin b;
  brin[v]=0;
  mark[v]=1;

  while (1)
      {vopp=0;
      Forall_adj_brins(b,v,G)
          {if(b>0) continue;
          if(!mark[G.vin[-b]]) { vopp=G.vin[-b]; break; }
          }
      if (!vopp)
          {if(!brin[v]) break;      // no path found.
          v=G.vin[-brin[v]];        // backtrack.
          continue;
          }
      brin[vopp]=-b;
      if (G.InDegree(vopp) < max_indeg-1)
          {while (brin[vopp])
              {elist.push(Abs(brin[vopp]));
              vopp = G.vin[-brin[vopp]];
              }
          return 1;
          }
      mark[vopp] = 1;
      v=vopp;
      }
  return 0;
  }

static void ReversePath(TopologicalGraph &G, IntList &elist)
  { while (!elist.empty()) G.ReverseEdge(elist.pop()); }

// orient the given graph so that max deg^-(v) shall be minimum.
int TopologicalGraph::InfOrient()
  {tvertex v;
  IntList vlist, elist;
  vlist.SetName("vlist");    elist.SetName("elist");
  int max_indeg;
  tedge e;
  Prop<bool> eoriented(Set(tedge()),PROP_ORIENTED,true);
  while (1)
      {max_indeg = 0;
      ForAllVerticesOfG(v)
          {if(InDegree(v) > max_indeg)
              {max_indeg = InDegree(v);
              vlist.clear();
              vlist.push(v());
              }
          else if(InDegree(v)==max_indeg)
              vlist.push(v());
          }
      if (max_indeg < 2)
          {ForAllEdgesOfG(e) eoriented[e] = true;
          return max_indeg+1;
          }
      while (!vlist.empty())
          {v = vlist.pop();
          if (!FindOrientedPath(*this,v,max_indeg,elist))
              {ForAllEdgesOfG(e) eoriented[e] = true;
              return max_indeg+1;
              }
          ReversePath(*this,elist);
          }
      vlist.clear();
      }
  }


