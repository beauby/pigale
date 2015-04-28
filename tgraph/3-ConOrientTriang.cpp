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



 
#include <TAXI/KantShel.h>
#include <TAXI/Tflist.h>
#include <TAXI/Tdebug.h>

static int MarkAngles(TopologicalGraph &G, svector<int> &mark);
static int TriangulateByMarks(TopologicalGraph &G, svector<int> &mark);
static int KantTriangulateAndThreeOrient(TopologicalGraph &G, tbrin fb);

int TopologicalGraph::TriconTriangulate()
  {if(!CheckPlanar())return 1;
  if (!CheckTriconnected()) return 2;
  svector<int> mark(-ne(),ne(),0);
  mark.SetName("mark");
  if (MarkAngles(*this,mark)) return -1;
  TriangulateByMarks(*this,mark);
  return 0; 
  }
int TopologicalGraph::Tricon3orient()
  {if(!CheckPlanar()) return 1;
  if (!CheckTriconnected()) return 2;
  int OrgM = ne();
  tbrin b = extbrin();
  if(KantTriangulateAndThreeOrient(*this,b)) return -1;
  Prop<bool> eoriented(Set(tedge()),PROP_ORIENTED,true);
  tedge e;
  for(e = ne();e > OrgM;e--) DeleteEdge(e); 
  for(e = 1;e <= ne();e++) eoriented[e] = true;
  return 0;
  }
static int MarkAngles(TopologicalGraph &G, svector<int> &mark)
// mark[b] is the #marks of the angle FOLLOWING the brin b in circular order
  {int OrgM = G.ne();
  if(KantTriangulateAndThreeOrient(G,G.extbrin()))return 1;
  tbrin b, bb;
  for(b = -G.ne();b < -OrgM;b++)  // for all ADDED incoming edges...
      {bb = b;
      do 
          bb = G.acir[bb];
      while (bb.GetEdge() > OrgM); // To put the marks on the original brins
      mark[bb] += 2;
      }
  for(;b <= -1;b++)  // for all ORIGINAL incoming edges...
      {mark[b]++;
      bb=b;
      do
          bb=G.acir[bb];
      while (bb.GetEdge()>OrgM);
      mark[bb]++;
      }
  tedge e;
  for(e = G.ne();e > OrgM;e--) G.DeleteEdge(e);
  return 0;
  }
static int TriangulateByMarks(TopologicalGraph &G, svector<int> &mark)
  {int OrgM=G.ne(), len;
     
  svector<int> MarkedB(-OrgM,OrgM,0);
  tbrin b, bb, b0, b1, b2, b3, b4, PrevB;
  tbrin NextB=0;
  for (b=-OrgM;b<=OrgM;b++)
      {if (MarkedB[b]) continue;
      bb=b;
      b0=0;
      len=0;
      // mark all the brins of the face.
      do
          {len++; MarkedB[bb]=1;
          if(!mark[bb])b0=bb;
          bb=-G.cir[bb];
          } while(b != bb);

      while(len > 3)
          {if(b0 != 0)  // if there is an angle of mark 0...
              {PrevB=G.acir[-b0];
              NextB=-G.cir[b0];
              G.NewEdge(PrevB, G.cir[NextB]);
              len--;
              mark[PrevB]--;
              mark(G.cir[NextB])=mark[NextB]-1;
              NextB=G.cir[NextB];
              if(!mark[PrevB]) b0=PrevB;
              else if(!mark[NextB]) b0=NextB;
              else { b0=0; bb=PrevB;
              }
              }
          else
              {b1=bb;// now no angle of mark 0
              PrevB=0;
              do // find  angle of mark 1 which satisfies the conditions.
                  {if(mark[b1]==1)
                      {PrevB=G.acir[-b1];
                      NextB=-G.cir[b1];
                      if (mark[PrevB]>=2 || mark[NextB]>=2) break;
                      }
                  b1=-G.cir[b1];
                  } while(b1!=bb);
              if(PrevB!=0)   // mark 1 found.
                  {G.NewEdge(PrevB, G.cir[NextB]);
                  len--;
                  mark[PrevB]--;
                  mark(G.cir[NextB])=mark[NextB]-1;
                  NextB=G.cir[NextB];
                  if(!mark[PrevB]) b0=PrevB;
                  else if(!mark[NextB]) b0=NextB;
                  else { b0=0; bb=PrevB; }
                  }
              else // all the marks are at least 2. Zigzag triangulation.
                  {b0 = bb;
                  while(1)
                      {b1=-G.cir[b0];
                      b2=-G.cir[b1];
                      b3=-G.cir[b2];
                      b4=-G.cir[b3];
                      if (b0==b3) break;  // the face is a triangle.
                      G.NewEdge(b0, G.cir[b2]);
                      if (b0==b4) break;  // the face was a square.
                      G.NewEdge(b0, G.cir[b3]);
                      b0=G.acir[-b0];
                      }
                  break;
                  }
              }
          }
      }

  return 0;
  }
static int KantTriangulateAndThreeOrient(TopologicalGraph &G, tbrin fb)
// This function triangulates and inf-3-orients the graph,
// using the Kant shelling.
// precondition: G has a plane circular order.
  {KantShelling KS(G,fb);
  if(KS.Error()){DebugPrintf("Error:KantShelling Creation"); return 1;} 
  int i;
  tvertex v;
  tbrin left, right, b, LastB, b1;
  IntList EdgesToReorient;
  flist<int> B1,B2;
  while (1)
      {i=KS.FindNext(left,right);
      if (i==0) break;
      if (i==1) // a vertex shelled.
          {v=G.vin[left];
          b1=left;
          if (left >0) EdgesToReorient.push(left());
          if (right > 0) EdgesToReorient.push(right());
          b=-G.acir[-left];
          LastB = G.cir[-right];
          while (b!=LastB)
              {b=G.acir[b];
              if (G.vin[-b]==v) // the edge exists.
                  {b1=-b;
                  if(b > 0) EdgesToReorient.push(b());
                  b=G.acir[b];
                  }
              else  // the edge does not exist.
                  {B1.push(b1());B2.push(G.cir[b]());}
              b=-b;
              }
          }
  
      else   // a face shelled.
          {b=left;
          while (b != right) 
              {if (b < 0) EdgesToReorient.push(-b);b=G.cir[-b];}
          if (right > 0) EdgesToReorient.push(right());
          b=-G.acir[left];
          LastB=G.cir[-right];
          while (b!=LastB)
              {B1.push(-left);B2.push(b());b=-G.acir[b];}
          b=-G.acir[right];
          while (b!=left)
              {B1.push(-right);B2.push(b());b=-G.acir[b];}
          }
      }

  // Add edges.
  while (!B1.empty())
      G.NewEdge(tbrin(B1.pop()),tbrin(B2.pop()));
  // Reorient edges.
  while (!EdgesToReorient.empty())
      G.ReverseEdge(EdgesToReorient.pop());

  // Then triangulate F_out.
  right = G.cir[-fb];  b = -G.cir[-right]; 
  LastB = G.acir[fb];
  // Edges are reverted when poped so we have to do that way
  while (b != LastB)   
      {G.NewEdge(-fb,G.cir[b]);
      b=-G.cir[G.cir[b]];  // as we get the new edge
      }

  return 0;
  }
