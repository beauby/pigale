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


#define LRMAP
#include <TAXI/DFSLow.h>
#include <TAXI/embed.h>

int DFSLow::Planarity()
  {if(!ne())return 1;
  _LrSort LrSort(nv(),ne());
  LralgoSort(LrSort);
  _Hist Hist(nv(),ne());
  int ret = Lralgo(LrSort,Hist);
  Embed Embedder(*this,LrSort,Hist);
  Embedder();
  Prop<tbrin> pbrin(PV(),PROP_PBRIN);
  pbrin[0]=0;
  pbrin[1]=1;
  for (tvertex v=2; v<= nv(); v++)
      pbrin[v]=-treein(v);
  if(ret)
      {Prop1<int> maptype(Set(),PROP_MAPTYPE);
      maptype() = PROP_MAPTYPE_LRALGO;
      Prop1<int> isplanar(Set(),PROP_PLANAR);
      //planarMap() = 1;
      }
  //else planarMap() = -1;
  return ret;
  }
        
int DFSLow::Lralgo(const _LrSort &LrSort, _Hist &Hist)
  {int n=nv();
  _Twit Twit(ne(),nv(),nvin, Hist);
  svector<tedge> ctel(LrSort.tel); ctel.SetName("ctel DFSGraph.cpp");
  tvertex vi, vii;
  tedge ej;
    
  // Going up in the tree along LrSort.tref edges
  vi = 1;
  while(LrSort.tref[vi]!=0) vi=treetarget(LrSort.tref[vi]); 

  for(;;)
      {if(ctel[vi] == 0)                                    // No Edge
          {if(vi == tvertex(1))
              return Twit.planar();
          vii =vi;
          vi = nvin[treein(vi)];                            // Bactracking to the father of vi
          Twit.Deletion(vi);                                // Delete cotree edges
          if(LrSort.tref[vi] == treein(vii)())              // Backtracking along a reference edge
              continue;
          if(status[treein(vii)] > PROP_TSTATUS_LEAF)       // Backtracking to a fork
              {if(status[treein(vii)] == PROP_TSTATUS_THIN) // Backtacking along a thin edge
                  {ej = Twit.Twin().Firstbot();
                  Hist.Dus[treein(vii)] = Hist.Flip[ej];    // tree edge side = side of ej
                  Twit.Thin(ej);                            // Merge THIN
                  }
              else                                          // Backtacking along a thin edge
                  {Twit.Thick();                            // Merge THICK
                  ej=Twit.Twin().lbot();
                  }
              Hist.Link[treein(vii)] = Twit.Twin().ltop();  // Hist.Link edge to left top
              Twit.NextFork();                              // Looking for nex fork
              Twit.Fusion(ej);                              // fusion
              }
          else                                              // Backtacking along an isthmus
              {Twit.NextFork();
              Hist.Link[treein(vii)] = -1;
              }
          }
      else if(ctel[vi] < n)                                 // Going up in the tree
          {vii = treetarget(ctel[vi]);
          Twit.NewFork(vi);                                 // New tree fork
          ctel[vi] = LrSort.linkt[ctel[vi]];
          vi = vii;                                         // updating current vertex
          while(LrSort.tref[vi]!=0)vi = treetarget(LrSort.tref[vi]);
          // Going up in the tree along LrSort.tref edges
          continue;
          }
      else
          {ej = ctel[vi];                                    // Treating a new cotree edge
          ctel[vi] = LrSort.linkt[ctel[vi]];
          Twit.NewTwin(ej);                                  // Create a new Twin
          if(!Twit.FirstLink())Twit.Fusion(ej);              // Fusion
          }
      }
  }
