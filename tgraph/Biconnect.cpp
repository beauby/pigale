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
#include <TAXI/color.h>
#include <TAXI/graphs.h>
#include <TAXI/IntList.h>
#include <TAXI/Tdebug.h>



static int SchnyderMarkAngles(TopologicalGraph &G, svector<int> &mark);
static void BipolarMarkAngles(TopologicalGraph& G,svector<int>& Mark,int NedgeConnect);
static int SchnyderBiconnectByMarks(TopologicalGraph &G, svector<int> &mark);
static int BipolarBiconnectByMarks(TopologicalGraph &G, svector<int> &mark);


int TopologicalGraph::Biconnect()
// Biconnection of a planar graph.
  {if(Set().exist(PROP_BICONNECTED))return 0;
  MakeConnected();
  RemoveLoops();
  if(!CheckPlanar())return -1;

  if(debug())DebugPrintf("Biconnected");
  svector<int> VMark(1,nv());
  svector<char> BMark(-ne(),ne());
  VMark.SetName("VMark");BMark.SetName("BMark");
  BMark.clear();
  IntList LinkBList;
  tbrin b0, b, b2, start_b;
  VMark.clear();
  int mark = 0;
  
  int m = ne();
  for(b0 = -m; b0 <= m; b0++) 
      {if(BMark[b0] || !b0)continue;
      b=b0;
      ++mark;
      do
          {VMark[vin[b]] = mark;
          b=-cir[b];
          }while(VMark[vin[b]] != mark && b != b0);
      
      if(b == b0)continue;  // no articulating vertex in that face.
      
      b = start_b = acir[-b];
      LinkBList.push(b());
      ++mark;
      do
          {BMark[b] = 1;
          if(VMark[vin[b]] == mark)       // already visited.
              {LinkBList.push(acir[-b]());
              ++mark;
              }
          VMark[vin[b]] = mark;
          }while((b = -cir[b]) != start_b);

      b = LinkBList.pop();
      while(!LinkBList.empty())
          {b2 = LinkBList.pop();
          NewEdge(b2, cir[b]);
          b=b2;
          }
      }
  Prop1<int> isbicon(Set(),PROP_BICONNECTED);
  return 0;
  }
/**********************************************************************************/
int TopologicalGraph::Opt6Biconnect()
// The graph must be simple
// augmentation to biconnected graph without changing the embedding.
  {if(Set().exist(PROP_BICONNECTED))return 0;
  if(!CheckSimple())return -1;
  MakeConnected();
  if(!CheckPlanar())return -1;
  if(debug())DebugPrintf("Opt6Biconnect");
  Prop<bool> oriented(Set(tedge()),PROP_ORIENTED,false);
  svector<bool> save_oriented(0,ne()); save_oriented.Tswap(oriented);
  svector<int> mark(-ne(),ne()); mark.SetName("mark");
  mark.clear();
  if(SchnyderMarkAngles(*this,mark))return -1;
  SchnyderBiconnectByMarks(*this,mark);
  save_oriented.resize(0,ne());
  save_oriented.Tswap(oriented);RestoreOrientation();
  //Prop1<int> isbicon(Set(),PROP_BICONNECTED);
  return 0;
  }

int SchnyderMarkAngles(TopologicalGraph &G, svector<int> &mark)
// mark[b] is the #marks of the angle FOLLOWING the brin b in positive circular order.
  {int OrgM = G.ne();
  if(G.ZigZagTriangulate() < 0) return -1;
  tbrin bb,b = 1;
  G.SchnyderOrient(b);

  
  for(b = -G.ne();b < -OrgM;b++)  // for all ADDED incoming edges...
      {bb = b;
      do
          { bb=G.acir[bb];
          }while (bb.GetEdge()>OrgM);
      mark[bb]+=2;
      }
  for(;b <= -1;b++)  // for all ORIGINAL incoming edges...
      {mark[b]++;
      bb=b;
      do
          { bb=G.acir[bb];
          }while (bb.GetEdge()>OrgM);
      mark[bb]++;
      }
  
  // set marks of three exterior angles to 2.
  b = 1;
  do
      {b = G.acir[b]; //avant cir
      }while(b.GetEdge() > OrgM);
  
  //mark[b]=2; mark[-1]= 2;
  mark[-1] = Max(mark[-1],2);
  mark[b] = Max(mark[b],2);
   
  b = -G.acir[-1];
  do
      {b = G.acir[b];//avant cir
      }while(b.GetEdge() > OrgM);
  //  mark[b]=2;
  mark[b] = Max(mark[b],2);
  
  // delete added edges.
  tedge e;
  for(e=G.ne();e>OrgM;e--) G.DeleteEdge(e);
  return 0;
  }
int SchnyderBiconnectByMarks(TopologicalGraph &G, svector<int> &mark)
  {svector<int> VMark(1,G.nv());
  int m = G.ne();
  svector<char> BMark(-m,m); BMark.clear();
  IntList LinkBList;
  tbrin b0, b, bb, b2, start_b;
  int LastLinkV,count,max_mark;
  
  for(b0 = -m; b0 <= m; b0++)  // for all brins...
      {if(BMark[b0] || !b0)continue;  // face  visited.
      VMark.clear();
      b=b0;
      do
          {VMark[G.vin[b]] = 1;
          b=-G.cir[b];
          }while(!VMark[G.vin[b]] && b!=b0);

      if(b == b0) continue;  // no articulating vertex.
      // Now, vin[b] is an articulating vertex.
      // look for a face without vertex of mark >= 2.
      VMark.clear();
      max_mark = 0;
      count = 1;
      bb = b;
      do
          {if(VMark[G.vin[bb]] == count)
              {if(max_mark <= 1)break;
              max_mark=0; count++;
              }
          else
              {max_mark = Max(max_mark,mark[bb]);
              VMark[G.vin[bb]] = count;
              }
          bb = -G.cir[bb];
          }while(bb != b);
      
      //if (bb==b)      // no face whose vertices have mark < 2.
      //    bb=G.acir[-b];
      // Now vin[bb] is an articulating vertex
      // and bb is the last brin of the face
      // without vertex of mark >= 2.
      // look for a vertex of mark >= 1.
      do
          {bb = G.acir[-bb];
          }while(mark[bb] < 1);

      // Now bb is a brin of a face without vertex of mark >= 2
      // such that mark[vin[bb]]=1.
      b = start_b = bb;
      VMark.clear();
      count=0;
      LastLinkV = 0;
      LinkBList.push(b());
      
      do     // loop for each face.
          {count++;
          BMark[b]=1;
          if (VMark[G.vin[b]])       // already visited.
              {if(LastLinkV < VMark[G.vin[b]])
                  {bb=b;
                  do
                      {bb=G.acir[-bb];
                      }while(mark[bb]<2 && G.vin[bb]!=G.vin[b]);
                  if (G.vin[bb] == G.vin[b])return -10;
                  LastLinkV = VMark[G.vin[bb]];
                  LinkBList.push(bb());
                  }
              }
          VMark[G.vin[b]] = count;
          }while((b =-G.cir[b]) != start_b);
      
      // link 2-blocks.
      b = LinkBList.pop();
      while(!LinkBList.empty())
          {b2 = LinkBList.pop(); G.NewEdge(b2, G.cir[b]);b = b2;}
      }
  return 0;
  }

/**********************************************************************************************/
/*
Totalement idiot:
on n'utilise pas Mark
Impossible sans changer le plongement
sauf si admet 3
*/
int TopologicalGraph::Opt2Biconnect()
  {if(Set().exist(PROP_BICONNECTED))return 0;
  RemoveLoops();
  MakeConnected();
  int NedgeConnect = ne();

  // To get a proper embedding
  if(!CheckPlanar())return -1;
  tbrin ExtBrin = extbrin();
  // The Marks are defined on the connected graph.
  // Mark[b] allows to insert a brin in cir order after brin
  svector<int> Mark(-ne(),ne()); Mark.SetName("2Biccon:Mark");
  Biconnect();
  BipolarPlan(ExtBrin);    //may call planarity
  ReverseEdge(ExtBrin.GetEdge());
  FixOrientation();// pour debug
  
  BipolarMarkAngles(*this,Mark,NedgeConnect);
  for(tedge e = ne(); e > NedgeConnect;e--)DeleteEdge(e);
  int ret = BipolarBiconnectByMarks(*this,Mark);
  if(!ret)Prop1<int> isbicon(Set(),PROP_BICONNECTED);
  return ret;
  }

void BipolarMarkAngles(TopologicalGraph& G,svector<int>& Mark,int NedgeConnect)
  {Mark.clear();
  tvertex v;
  tbrin b0,b,bleft,bright;
  // bleft = leftmost incoming true edge
  // bright = rightmost outgoing true edge
  ForAllVertices(v,G)
      { b0 = b = bleft = G.pbrin[v];
      while(bleft < 0) bleft = G.cir[bleft];// Looking for a positive brin
      while(bleft > 0) bleft = G.cir[bleft];
      // Look for original brin
      while (bleft.GetEdge()() > NedgeConnect) bleft = G.cir[bleft];
      Mark[bleft] += 1;
      b0 = b = bright = G.pbrin[v];
      while(bright < 0) bright = G.cir[bright];
      // Look for original brin
      while(bright.GetEdge()() > NedgeConnect) bright = G.cir[bright];
      Mark[bright] += 1;
      }
  }

int BipolarBiconnectByMarks(TopologicalGraph& G,svector<int> &Mark)
  {svector<tbrin> & Fpbrin = G.ComputeFpbrin();
  int NFaces = Fpbrin.n();
  svector<int> VMark(1,G.nv()); VMark.clear(); VMark.SetName("Opt2:VMark");
  tbrin b0,b,bb;
  tedge e;
  int mark = 0;
  IntList BList;
  int bad = 0;
  
  for(int i = 1;i <= NFaces;i++)
      {b = b0 = Fpbrin[i];
      //printf("\n face:%d Fpbrin=%d\n",i,b());
      ++mark;
      int firsttime = 1;
      do
          {if(VMark[G.vin[b]] == mark)
              {bb = b;
              bb = -G.acir[bb];
              ++mark;
              if(firsttime && Mark[bb])
                  {b0 = b = bb;
                  //printf(" firsttime bb=%d\n",bb());
                  --Mark[bb];
                  firsttime = 0;
                  }
              else if(firsttime)
                  {bad = 1;
                  //printf("IMPOSSIBLE ? error first time %d\n",bb());
                  }
              else
                  {//printf(" secondtime bb=%d\n",bb());
                  if(Mark[bb] > 1)
                      {BList.push(bb());
                      --Mark[bb];
                      }
                  else
                      {//printf("error second time %d\n",bb());
                      bad = 1;
                      }

                  }
              }
          VMark[G.vin[b]] = mark;
          }while((b = G.cir[-b]) != b0);
      
      b = G.acir[b0];
      while (!BList.empty())
          {bb = BList.pop_last();
          //b = G.NewEdge(b,bb).firsttbrin();
          //printf("ADD %d %d\n",G.vin[b](),G.vin[bb]());
          b = G.NewEdge(b,bb).secondtbrin();
          }
      }
  delete &Fpbrin;
  return bad;
  }
