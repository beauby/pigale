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
#include <TAXI/graph.h>
#include <TAXI/graphs.h>
#include <TAXI/bicon.h>
#define PRIVATE 
#include <TAXI/lralgo.h>
#undef PRIVATE


void LralgoSort(int n, int m, svector<tvertex> &vin, const _Bicon &Bicon ,const svector<tvertex> &low,_LrSort &LrSort)
  {svector<tedge> thin(0,n); thin.clear();
  thin.SetName("thin Lr-Algo.cpp");
  svector<tedge> thick(0,n); thick.clear();
  thick.SetName("thick Lr-Algo.cpp");
  tedge je,pje,nextje;
  tvertex iv;

  TEdgeStackPartition Stack(LrSort.linkt);
  // filling piles thin and thick (sort by Bicon.low)
  for(je = 1;je < n;je++) // tree edges 
      {iv = low[je+1];
      if(Bicon.status[je] == PROP_TSTATUS_THICK)
          Stack.Push(thick[iv],je);
      else
          Stack.Push(thin[iv],je);
      }
  for(je = n;je <= m;je++) // cotree edges 
      Stack.Push(thin[vin[je.firsttbrin()]],je);

  // making the list of edges: filling LrSort.tel 
  for(iv = n;iv >= 1;iv--)
      {je = thick[iv];
      while(je!=0)
          { nextje = Stack.Next(je);
          Stack.Push(LrSort.tel[vin[je.firsttbrin()]],je);
          je = nextje;
          }
      je = thin[iv];
      while(je!=0)
          { nextje = Stack.Next(je);
          if(je < n)
              Stack.Push(LrSort.tel[vin[je.firsttbrin()]],je);
          else
              Stack.Push(LrSort.tel[vin[je.secondtbrin()]],je);
          je = nextje;
          }
      }

  // assigning a tremaux reference tree edge to each non-terminal vertex: filling LrSort.tref
  for(iv=1;iv <= n;iv++)
      {if ((je = LrSort.tel[iv])==0) continue;
      if (je <n) 
          {LrSort.tref[iv]=je; Stack.Pop(LrSort.tel[iv]); 
          continue;
          }
      while(je >= n)
          {pje =je;	je = Stack.Next(je);}
      if(je == 0)continue;
      LrSort.tref[iv] = je;
      /* a tree edge is found */
      Stack.PopNext(pje);
      }
  }
int lralgo(int n, int m, svector<tvertex> &vin,const _Bicon &Bicon, const _LrSort &LrSort, _Hist &Hist,bool OnlyTest)
// LrSort.tref[vertex] -> the DFS reference edge 
// LrSort.tel[vertex]  -> the first not treated edge at the vertex 
  {
  _Twit Twit(m,n,vin, Hist);
  // we use a copy of LrSort.tel, as we shall need it for computiong the map
  svector<tedge> ctel(LrSort.tel); ctel.SetName("ctel Lr-Algo.cpp");
  tvertex vi, vii;
  tedge ej;

  // Going up in the tree along LrSort.tref edges
  vi = 1;
  while(LrSort.tref[vi]!=0) vi=treetarget(LrSort.tref[vi]); 

  for(;;)
      {if(ctel[vi] == 0)                                    // No more Edge
          {if(vi == 1)return Twit.planar();                 // No bactracking from 1
          if(OnlyTest && !Twit.planar())return Twit.planar();
          vii =vi;
          vi = vin[treein(vi)];                             // Bactracking to the father of vi
          Twit.Deletion(vi);                                // Delete cotree edges
          if(LrSort.tref[vi] == treein(vii))                // Backtracking along a reference edge
              continue;
          if(Bicon.status[treein(vii)] > PROP_TSTATUS_LEAF) // Backtracking to a fork
              {if(Bicon.status[treein(vii)] == PROP_TSTATUS_THIN) // Backtacking along a thin edge
                  {ej = Twit.Twin().Firstbot();
                  Hist.Dus[treein(vii)] = Hist.Flip[ej];    // tree edge side = side of ej
                  Twit.Thin(ej);                            // Merge THIN
                  }
              else                                          // Backtacking along a thin edge
                  {Twit.Thick();                            // Merge THICK
                  ej=Twit.Twin().lbot();
                  }
              Hist.Link[treein(vii)] = Twit.Twin().ltop();  // Hist.Link edge to left top
              Twit.NextFork();                              // We remove the fork and Look for nex fork
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

int fastlralgo(int n, int m,svector<tvertex> &vin,const _Bicon &Bicon, _LrSort &LrSort, _FastHist &Hist)
  {
  _FastTwit Twit(m,n,vin, Hist);
  svector<tedge> &ctel = LrSort.tel;
  tvertex vi, vii;
  tedge ej;
  int ncotree;

  // Subcalls do not need destructor call. Hence, we may use setjmp/longjmp facility.

  int ret_val;
  if ((ret_val = setjmp(Twit.env))!=0)
      return Twit.planar();
  // Going up in the tree along LrSort.tref edges
  ncotree = 0;
  vi = 1;
  while(LrSort.tref[vi]!=0) vi=treetarget(LrSort.tref[vi]); 
  vii = vi; 
  for(;;)
      {if(ctel[vi] >= n) 
          {ej = ctel[vi];                                    // Treating a new cotree edge
          ctel[vi] = LrSort.linkt[ctel[vi]];
          Twit.NewTwin(ej);                                  // Create a new Twin
          LrSort.num[ej] = ++ncotree;
          if(vi!=vii && !Twit.FirstLink())
              Twit.Fusion(ej);                              // Fusion
          }
      else if(ctel[vi] == 0)                                    // No Edge
          {if(vi == tvertex(1))return Twit.planar();
          vii =vi;
          vi = vin[treein(vi)];                             // Bactracking to the father of vi
          Twit.Deletion(vi);                                // Delete cotree edges

          if(LrSort.tref[vi] == treein(vii))                // Backtracking along a reference edge
              continue;
          if(Bicon.status[treein(vii)] > PROP_TSTATUS_LEAF) // Backtracking to a fork
              {if(Bicon.status[treein(vii)] == PROP_TSTATUS_THIN) // Backtacking along a thin edge
                  {ej = Twit.Twin().Firstbot();
                  Twit.Thin(ej);                            // Merge THIN
                  }
              else                                          // Backtacking along a thin edge
                  {Twit.Thick();                            // Merge THICK
                  ej=Twit.Twin().lbot();
                  } 
              Twit.NextFork();                              // Looking for nex fork
              Twit.Fusion(ej);                              // fusion
              }
          else                                              // Backtraking along an articulation
              Twit.NextFork();
          }
      else                                                 // Going up in the tree
          {vii = treetarget(ctel[vi]);
          Twit.NewFork(vi);                                 // New tree fork
          ctel[vi] = LrSort.linkt[ctel[vi]];
          while(LrSort.tref[vii]!=0)vii = treetarget(LrSort.tref[vii]);
          // Going up in the tree along LrSort.tref edges
          vi = vii;
          continue;
          }
      }
  } 
void NewLralgoSort(int n, int m, svector<tvertex> &vin, const _Bicon &Bicon ,const svector<tvertex> &low,_LrSort &LrSort)
  {svector<tedge> thin(0,n); thin.clear();
  thin.SetName("thin Lr-Algo.cpp");
  svector<tedge> thick(0,n); thick.clear();
  thick.SetName("thick Lr-Algo.cpp");
  tedge je,pje,nextje;
  tvertex iv;

  TEdgeStackPartition Stack(LrSort.linkt);
  // filling piles thin and thick (sort by Bicon.low)
  for(je = 1;je < n;je++) // tree edges 
      {iv = low[je+1];
      if(Bicon.status[je] == PROP_TSTATUS_THICK)
          Stack.Push(thick[iv],je);
      else
          Stack.Push(thin[iv],je);
      }
  for(je = n;je <= m;je++) // cotree edges 
      Stack.Push(thin[vin[je.firsttbrin()]],je);

  // making the list of edges: filling LrSort.tel 
  for(iv = n;iv >= 1;iv--)
      {je = thick[iv];
      while(je!=0)
          { nextje = Stack.Next(je);
          Stack.Push(LrSort.tel[vin[je.firsttbrin()]],je);
          je = nextje;
          }
      je = thin[iv];
      while(je!=0)
          { nextje = Stack.Next(je);
          if(je < n)
              Stack.Push(LrSort.tel[vin[je.firsttbrin()]],je);
          else
              Stack.Push(LrSort.tel[vin[je.secondtbrin()]],je);
          je = nextje;
          }
      }

  // assigning a tremaux reference tree edge to each non-terminal vertex: filling LrSort.tref
  for(iv=1;iv <= n;iv++)
      {if ((je = LrSort.tel[iv])==0) continue;
      LrSort.tref[iv] = je; 
      Stack.Pop(LrSort.tel[iv]); 
      }
  }
int Newlralgo(int n, int m, svector<tvertex> &vin,const _Bicon &Bicon, const _LrSort &LrSort, _Hist &Hist,bool OnlyTest)
// LrSort.tref[vertex] -> the DFS reference edge 
// LrSort.tel[vertex]  -> the first not treated edge at the vertex 
  {
  _NewTwit Twit(m,n,vin, Hist);
  // we use a copy of LrSort.tel, as we shall need it for computiong the map
  svector<tedge> ctel(LrSort.tel); ctel.SetName("ctel Lr-Algo.cpp");
  tvertex vi, vii;
  tedge ej;

  //for(tedge ej =1 ; ej <= m;ej++)    DebugPrintf("%d  %d %d",ej(),vin[ej](),vin[-ej]());

  vi = 1;
  while((ej = LrSort.tref[vi])!=0 && ej < n ) vi=treetarget(ej);
  //while((ej = LrSort.tref[vi])!=0 && ej < n ) {vi=treetarget(ej);DebugPrintf("Climbing to:%d",vi());}
  if(ej >= n)Twit.NewTwin(ej);                               // Create a new Twin
   //DebugPrintf("edge:%d  %d %d",ej(),vin[ej](),vin[-ej]());
  for(;;)
      {if(ctel[vi] == 0)                                    // No more Edge
          {if(vi == 1)return Twit.planar();                 // No bactracking from 1
          if(OnlyTest && !Twit.planar())return Twit.planar();
          vii =vi;
          vi = vin[treein(vi)];                             // Bactracking to the father of vi
          //DebugPrintf("Backtracking to:%d",vi());
          Twit.Deletion(vi);                                // Delete cotree edges
          if(LrSort.tref[vi] == treein(vii))                // Backtracking along a reference edge
              continue;
          if(Bicon.status[treein(vii)] > PROP_TSTATUS_LEAF) // Backtracking to a fork
              {if(Bicon.status[treein(vii)] == PROP_TSTATUS_THIN) // Backtacking along a thin edge
                  {ej = Twit.Twin().Firstbot();
                  Hist.Dus[treein(vii)] = Hist.Flip[ej];    // tree edge side = side of ej
                  Twit.Thin(ej);                            // Merge THIN
                  }
              else                                          // Backtacking along a thin edge
                  {Twit.Thick();                            // Merge THICK
                  ej=Twit.Twin().lbot();
                  }
              Hist.Link[treein(vii)] = Twit.Twin().ltop();  // Hist.Link edge to left top
              Twit.NextFork();                              // We remove the fork and Look for nex fork
              //DebugPrintf("FUSION %d  %d %d",ej(),vin[ej](),vin[-ej]());
              Twit.Fusion(ej);                              // fusion
              }
          else                                              // Backtacking along an isthmus
              {Twit.NextFork();
              Hist.Link[treein(vii)] = -1;
              }
          }
      else if(ctel[vi] < n)                                 // Going up in the tree
          {vii = treetarget(ctel[vi]);
          //DebugPrintf("Climbing to:%d",vii());
          Twit.NewFork(vi);                                 // New tree fork
          ctel[vi] = LrSort.linkt[ctel[vi]];
          vi = vii;                                         // updating current vertex
          while((ej = LrSort.tref[vi])!=0 && ej < n ) vii = vi = treetarget(ej);
          //while((ej = LrSort.tref[vi])!=0 && ej < n ) {vii = vi = treetarget(ej);DebugPrintf("Climbing to:%d",vi());}
          vii = vi;
          //if(ej >= n){Twit.NewTwin(ej);DebugPrintf("NEWTWIN A:%d  %d %d",ej(),vin[ej](),vin[-ej]());}
          if(ej >= n)Twit.NewTwin(ej);
          // Going up in the tree along LrSort.tref edges
          continue;
          }
      else
          {ej = ctel[vi];                                    // Treating a new cotree edge
          //DebugPrintf("NEWTWIN B:%d  %d %d",ej(),vin[ej](),vin[-ej]());
          ctel[vi] = LrSort.linkt[ctel[vi]];
          Twit.NewTwin(ej);                                  // Create a new Twin
          if(!Twit.FirstLink())Twit.Fusion(ej);              // Fusion
          }
      }
  }



int Newfastlralgo(int n, int m,svector<tvertex> &vin,const _Bicon &Bicon, _LrSort &LrSort)
  {
  _NewFastTwit Twit(m,n,vin);
  svector<tedge> &nonRefEdges = LrSort.tel;
  svector<tedge> &refEdge = LrSort.tref;
  tedge ej;
  tedge nonRefEdge;
  tedge backEdge; // tree edge along which we are bactracking
  int ret_val;
  if((ret_val = setjmp(Twit.env))!=0)
      return Twit.planar();

  // Going up in the tree along refEdge edges
  tvertex vi = 1;
  while((ej = refEdge[vi])!=0 && ej < n ) vi = treetarget(ej); 
  if(ej >= n) Twit.NewTwin(ej); 

  for(;;)
      {nonRefEdge=nonRefEdges[vi];
      if(nonRefEdge == 0)                            // all edges at iv had been treated
          {backEdge = treein(vi);
          if(vi == tvertex(1))
              return Twit.planar();
          vi = vin[backEdge];                        // Bactracking to the father of vi
          Twit.Deletion(vi);
          if(refEdge[vi] == backEdge)                // Backtracking along a reference tree edge
              continue;
          switch(Bicon.status[backEdge])
              {case PROP_TSTATUS_THIN:               // Backtacking along a thin edge
                  Twit.Thin();                     // Merge THIN
                  continue;
              case PROP_TSTATUS_THICK:               // Backtacking along a thick edge
                  Twit.Thick();                      // Merge THICK                  
                  continue;
              default:                               // Backtraking along an isthmus
                  Twit.PopFork();
                  continue;
              }
          }
      nonRefEdges[vi] = LrSort.linkt[nonRefEdge];
      // Going up in the tree
      if (nonRefEdge<n) 
          {nonRefEdges[vi] = LrSort.linkt[nonRefEdge];
          Twit.NewFork();                         // New tree fork
          vi = treetarget(nonRefEdge);
          while((ej = refEdge[vi])!=0 && ej < n ) vi = treetarget(ej); 
          if(ej >= n) Twit.NewTwin(ej);               // Create a new Twin
          }
      // Cotree edge
      else Twit.Fusion(nonRefEdge);
      }
  }
