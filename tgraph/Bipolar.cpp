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

#include <TAXI/graphs.h>
#include <TAXI/Tmessage.h>
#include <TAXI/color.h>
#include <TAXI/DFSGraph.h>
// Extends a bipolar oriented partial subgraph of G to G.
// G is assumed to be 2-connected.
// The vertices belonging to the bipolarly oriented partial subgraph
// have a vmark; while the oriented edges are those with PROP_ORIENTED.
// Edges might be reoriented in input.
// tbrin bst is incident to the source and -bst is incident to the sink.
void DrawGraph(Graph &G);
bool FindPath(TopologicalGraph &G, svector<bool> &vmark, tbrin b0, 
	      svector<tbrin> &Lpath, int &Llen);
void BipExtend(TopologicalGraph &G, tbrin bst, svector<bool> &vmark)
  {// Compute the indegrees of vertices
  int n=G.nv();
  int m=G.ne();
  tvertex v,w;
  tbrin b,b0;
  tedge e;
  Prop<bool> Oriented(G.Set(tedge()),PROP_ORIENTED);
  Prop<bool> Reoriented(G.Set(tedge()),PROP_REORIENTED);
  svector<int> din(1,n);
  din.clear();
  
  for (b=-m;b<0;b++)
      {
          if (Oriented[b.GetEdge()])
              {if (Reoriented[b.GetEdge()])
                  din[G.vin[-b]]++;
              else
                  din[G.vin[b]]++;
              }    
      }
  // only the source should be marked and have din=0
  #ifdef TDEBUG
  for (v=1; v<=n; v++)
      if (din[v]==0)
          {if (vmark[v] && (v!=G.vin[bst]))
	    {
	      Tprintf("Additional source %d",v());
              return;
	    }
          }
      else
          {if (!vmark[v])
	    {
              Tprintf("s=%d is not a source",v());
	      return;
	    }
          }
  #endif
  // Algorithm: (queue,top,bot) is the queue of minimal vertices. 
  // we pick a minimal vertex and look at incident non
  // oriented edges.
  // Each such edge is the first edge of a non-orirented path 
  // which brins are queued in (Lpath,Lbot,Ltop)

  svector<tvertex> queue(0,2*m); int bot=0; int top=0;
  svector<tbrin> Lpath(0,m); int Llen;
  svector<bool> processed(0,n); processed.clear();
  tbrin nb;
  queue[top++]=G.vin[bst];
  while (bot<top)
      {v=queue[bot++]; // minimal vertex ?
      if (processed[v]) continue;
#ifdef TDEBUG
      if (din[v]!=0) return; //error
#endif
      b=b0=G.pbrin[v];
      do
          {if (!Oriented[b.GetEdge()])
              { // Find a path to a marked vertex
              if (!FindPath(G,vmark,b,Lpath,Llen))
                  return; // error
              while (Llen--)
                  {nb=Lpath[Llen];
                  Oriented[nb.GetEdge()]=true;
                  if (nb.out())
                      Reoriented[nb.GetEdge()]=true;
                  vmark[G.vin[nb]]=true;
                  ++din[G.vin[nb]];
                  }
              }
          w=G.vin[-b];
          if (b.out()^Reoriented[b.GetEdge()])
              {if (--din[w]==0)
                  queue[top++]=w;
#ifdef TDEBUG
              if (din[w]<0) return; // error
#endif
              }
          b=G.cir[b];
          } while (b!=b0);
      processed[v]=true;
      }
  }
void SortCir(TopologicalGraph &G, svector<tbrin> &ncir, svector<tbrin>
	     &npbrin, svector<int> &colore, int ncolore);
void DFSTree(TopologicalGraph &G, tvertex v0);
void BFSTree(TopologicalGraph &G, tvertex v0, svector<int> &order, int maxo);

// F is a mapping from V to int, connected components with constant F
// correspond to the vertices of the minor.
// The bipolar orientation of the minor is assumed to be implied by F.
// Moreover, min F is only reached at vin[bst] and max F is only reached 
// at vin[-bst].
void BipExtendMinor(TopologicalGraph &G, tbrin bst, svector<int> &F)
{
  tvertex s=G.vin[bst];
  tvertex t=G.vin[-bst];
  int n=G.nv();
  int m=G.ne();
  tbrin b,bb;
  tvertex v,w;
  tedge e; 
  //Prop<short> ecolor(G.Set(tedge()),PROP_COLOR);
  //Prop<short> vcolor(G.Set(tvertex()),PROP_COLOR);
  // Compute priority of edges
  svector<int> order(0,m); order[0]=0;
  //Prop<int> width(G.Set(tedge()),PROP_WIDTH); width.clear();
  for (e=1; e<=m; ++e)
    {if (F[G.vin[e.firsttbrin()]]==F[G.vin[e.secondtbrin()]])
      order[e]=0;
    else
      order[e]=1;
    }
  // Compute the tree
  BFSTree(G,s,order,1);
  Prop<bool> istree(G.Set(tedge()),PROP_ISTREE);

  // Remove all non internal edges
  for (e=1; e<=m; ++e)
    if (istree[e] && (order[e]!=0))
      {
	istree[e]=false;
      }
  // Show trees
//   for (e=1; e<=m; ++e)
//     if (istree[e]) width[e]=3;
  // Orient external edges and compute status
  svector<int> status(0,n); status.clear();
  Prop<bool> oriented(G.Set(tedge()),PROP_ORIENTED); 
  oriented.clear();
  Prop<bool> reoriented(G.Set(tedge()),PROP_REORIENTED); 
  reoriented.clear();
  for (e=1; e<=m; ++e)
    if (order[e]!=0)
      { oriented[e]=true;
//       ecolor[e]=Red;
      b = e.firsttbrin();
      v = G.vin[b]; w=G.vin[-b];
      if (F[v]>F[w])
	{ v=w; w=G.vin[b]; reoriented[e]=true;}
      status[w]|=1;
      status[v]|=2;
      }
  // Compute degree in the forest
  svector<int> deg(0,n); deg.clear();
  for (e=1; e<=m; ++e)
    if (istree[e])
      {++deg[G.vin[e.firsttbrin()]];
      ++deg[G.vin[e.secondtbrin()]];
      }
  // Fills List[0] to List[3]
  svector<tvertex> List(0,3); List.clear();
  svector<tvertex> ListLink(0,n); ListLink.clear();
  for (v=1; v<=n; ++v)
    if (deg[v]<=1)
      { // extremal or isolated vertex; add to corresponding list
	ListLink[v]=List[status[v]];
	List[status[v]]=v;
      }
  // At first, all the vertices assumed to become oriented.
  svector<bool> vmark(0,n);
  for (v=1; v<=n; ++v) vmark[v]=true;

  // First round: empty List[0] and unmark vertices
  while ((v=List[0])!=0)
    { List[0]=ListLink[v]; // pop v
    vmark[v]=false;
//     vcolor[v]=Yellow;
    // update deg for neighbours
    b = bb = G.pbrin[v];
    do 
      {if (istree[b.GetEdge()])
	{w=G.vin[-b];
	if (--deg[w]==1) // becomes extremal
	  { ListLink[w]=List[status[w]];
	  List[status[w]]=w;
	  }
	// remark: isolated have been extremal => pushed
	istree[b.GetEdge()]=false;
	}
      b=G.cir[b];
      } while (b!=bb);
    }
  // second round: empty List[1]
  while ((v=List[1])!=0)
    { List[1]=ListLink[v]; // pop v
    // update deg for neighbours and orient from v
    b = bb = G.pbrin[v];
//     vcolor[v]=Green;
    do 
      {e=b.GetEdge();
      if (istree[e])
	{oriented[e]=true;
// 	ecolor[e]=Green;
	if (b.in()) reoriented[e]=true;
	w=G.vin[-b];
	status[w] |= 1;
	if (--deg[w]==1) // becomes extremal
	  { ListLink[w]=List[status[w]];
	  List[status[w]]=w;
	  }
	// remark: isolated have been extremal => pushed
	istree[e]=false;
	}
      b=G.cir[b];
      } while (b!=bb);    
    }
  // Last round: empty List[3] through List[2]
  do
    {while ((v=List[2])!=0)
      { List[2]=ListLink[v]; // pop v
//       vcolor[v]=Violet;
      // update deg for neighbours and orient from v
      b = bb = G.pbrin[v];
      do 
	{e=b.GetEdge();
	if (istree[e])
	  {oriented[e]=true;
// 	  ecolor[e]=Violet;
	  if (b.out()) reoriented[e]=true;
	  w=G.vin[-b];
	  status[w] |= 2;
	  if (--deg[w]==1) // becomes extremal
	    { ListLink[w]=List[status[w]];
	    List[status[w]]=w;
	    }
	  // remark: isolated have been extremal => pushed
	  istree[e]=false;
	  }
	b=G.cir[b];
	} while (b!=bb);    
      }
    // pop isolated vertices from List[3]
    while ((v=List[3])!=0 && deg[v]==0)
      {//vcolor[v]=Cyan;
      List[3]=ListLink[v];
      }

    // Move one vertex from List[3] to List[2]
    if ((v=List[3])!=0)
      {	List[3]=ListLink[List[3]];
	ListLink[v]=List[2]; List[2]=v;
      }
    } while (v!=0);
  // finish extention
  BipExtend(G,bst,vmark);
}

int NPBipolar(TopologicalGraph &G, tbrin bst)
{GraphContainer DFSContainer; 
 DFSGraph DG(DFSContainer,G,bst);
 svector<bool> reoriented(1,DG.ne()); reoriented.SetName("Bipolar reorientation"); reoriented.clear();
 svector<bool> mark(1,DG.nv()); mark.SetName("Bipolar:mark"); mark.clear();
 Prop<bool> ireor(G.Set(tedge()),PROP_REORIENTED); ireor.clear();
 Prop<bool> ior(G.Set(tedge()),PROP_ORIENTED,true);

 for (tbrin b=1; b<=G.ne(); b++)
   {tedge ee=DG.ib(b).GetEdge();
   ior[b]=true;
   if (DG.ib(b)<0) G.ReverseEdge(ee);
   }
 mark[1]=mark[2]=true;
 // in what follows, z and b are positive.
 for (tbrin z=DG.nv(); z<=DG.ne(); z++)
   {bool re=reoriented[DG.uptree[z]];
   if (re) {G.ReverseEdge(DG.ie(z())); reoriented[z]=true;}
   re = !re;
   tvertex v=DG.nvin[-z];
   while (!mark[v])
     { tbrin b=DG.treein(v);
     mark[v]=true;
     if (re) {G.ReverseEdge(DG.ie(b())); reoriented[b]=true;}
     v=DG.nvin[b];
     }
   }
 return 0;
}
