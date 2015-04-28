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
#include <TAXI/color.h>
#include <TAXI/Tmessage.h>

// unused anymore
int OldBipPlanarize(TopologicalGraph &G, svector<tvertex> &orig)
  {int n=G.nv();
  int m=G.ne();
  tedge e;
  tvertex v;
  orig=G.vin;
  svector<tbrin> topin(0,n);
  svector<tvertex> order(0,n-1);
  svector<bool> mark(0,m); mark.clear();
  G.TopSort(topin,order);
  Prop<short> ecolor(G.Set(tedge()),PROP_COLOR);
  for (v=1; v <= n; v++)
      if (topin[v]!=0)
          { e = topin[v].GetEdge();
          mark[e]=true;
          }
  // Find a reverse-tree rooted at t
  //   svector<tbrin> topin2(0,n);
  //   svector<tvertex> order2(0,n-1);
  //   G.TopSort(topin2,order2,true);

  for (e=1; e<=m; e++)
      if (!mark[e])
          {
              G.MoveBrin(e.secondtbrin(),order[n-1]);
          }
  // Sort cotree edges
  svector<tedge> eorder(0,m-n);
  int ei=0;
  int i;
  tbrin b;
  svector<tedge> link(0,m);
  svector<tedge> top(0,n); top.clear();
  tbrin b0;
  tvertex w;
  for (i=0; i<n; i++)
      { v = order[i];
      b0 = b = G.pbrin[v];
      do 
          {if (b.out() && !mark[b.GetEdge()])
              { e=b.GetEdge();
              w = orig[-b];
              link[e]=top[w];
              top[w]=e;
              }
          b=G.cir[b];
          } while (b!=b0);
      }
  for (i=n-1; i>=0; i--)
      { v=order[i];
      e = top[v];
      while (e!=0)
          { eorder[ei++]=e;
          e=link[e];
          }
      }
  // Main loop on cotree edges
  bool again;
  do
      {
          again=false;
          for (i=ei-1; i>=0; i--)
              {
                  tedge e=eorder[i];
                  tvertex v=orig[e.secondtbrin()];
                  if (v!=G.vin[e.secondtbrin()])
                      {
                          G.MoveBrin(e.secondtbrin(),v);
                          if (!G.TestPlanar())
                              G.MoveBrin(e.secondtbrin(),order[n-1]);
                          else
                              again=true;
                      }
                  //    bool IsP;
                  //       do
                  // 	{
                  // 	  G.MoveBrin(e.secondtbrin(),v);
                  // 	  IsP=G.TestPlanar();
                  // 	  tedge f = topin2[v].GetEdge();
                  //           v = G.vin[f.secondtbrin()];
                  // 	} while (!IsP);
              }
      } while (again);
  return 0;
  }

int UpperList(TopologicalGraph &G, tvertex v, svector<tvertex> &list)
  {
  int nb=0; int top=0;
  int n=G.nv();

  svector<bool> mark(0,n); mark.clear();
  tvertex x=v;
  tvertex y;
  tbrin b,b0;
  do 
      {b=b0=G.pbrin[x];
      do 
          {if (b.out())
              if (!mark[y=G.vin[-b]])
                  {mark[y]=true;
                  list[nb++]=y;
                  }
          b=G.cir[b];
          } while(b!=b0);
      x=list[top++];
      } while(top<=nb);
  return nb;
  }
int UpperList(TopologicalGraph &G, tvertex v, svector<int> &y,svector<tvertex> &list)
  {
  int nb=0;
  int n=G.nv();
  int yv=y[v];
  int yx;
  tvertex x;
  svector<tvertex> link(0,n); link.clear();
  svector<tvertex> topv(yv,n); topv.clear();
  for (x=1; x<=n; x++)
    { if ((yx=y[x])>=yv)
      {link[x]=topv[yx];
      topv[yx]=x;
      }
    }
  for (int i=yv+1; i<=n; i++)
    { if ((x=topv[i])!=0)
      do 
	{
	  list[nb++]=x;
	} while ((x=link[x])!=0);
    }
  if ((x=topv[yv])!=0)
    do 
      {
	list[nb++]=x;
      } while ((x=link[x])!=0);
  return nb;
  }
int LowerList(TopologicalGraph &G, tvertex v, svector<tvertex> &list)
  {
  int nb=0; int top=0;
  int n=G.nv();

  svector<bool> mark(0,n); mark.clear();
  tvertex x=v;
  tvertex y;
  tbrin b,b0;
  do 
      {b=b0=G.pbrin[x];
      do 
          {if (b.in())
              if (!mark[y=G.vin[-b]])
                  {mark[y]=true;
                  list[nb++]=y;
                  }
          b=G.cir[b];
          } while(b!=b0);
      x=list[top++];
      } while(top<=nb);
  return nb;
  }
int BipPlanarize(TopologicalGraph &G, svector<tbrin>
                  &topin, svector<tvertex> &order,
                  svector<tvertex> &orig, tbrin bst, svector<int> &y) 
  {int n=G.nv();
  int m=G.ne();
  tedge e;
  tvertex v;
  orig=G.vin;
  
  svector<bool> mark(0,m); mark.clear();
  for (v=1; v <= n; v++)
      if (topin[v]!=0)
          { e = topin[v].GetEdge();
          mark[e]=true;
          }
  
  // try to planarize G
  
  tvertex s=order[0];
  tvertex t=order[n-1];
  tbrin b,b0;
  
  // Sort cotree edges
  svector<tedge> eorder(0,m-n);
  int ei=0;
  int i,j;
  svector<tedge> link(0,m);
  svector<tedge> top(0,n); top.clear();
  tvertex w;
  for (i=n-1; i>=0; i--)
      { v = order[i];
      b0 = b = G.pbrin[v];
      do 
          {if (b.out() && !mark[b.GetEdge()])
              { e=b.GetEdge();
              w = orig[-b];
              link[e]=top[w];
              top[w]=e;
              }
          b=G.cir[b];
          } while (b!=b0);
      }
  for (i=n-1; i>=0; i--)
      { v=order[i];
      e = top[v];
      while (e!=0)
          { eorder[ei++]=e;
          e=link[e];
          }
      }
  
  // mark st-edge for deletion
  mark[bst.GetEdge()]=true;
  
  GraphContainer GC(G.Container());
  TopologicalGraph G0(GC);
  Prop<short> ecolor(G0.Set(tedge()),PROP_COLOR);  
  for (e=m; e>0; e--)
      ecolor[e]=Black;


  // erase all but the tree and (s,t) edge
  for (e=m; e>0; e--)
      if (!mark[e])
	G0.DeleteEdge(e);
  // reinsert edges in the good order
  tedge e0;
  svector<tvertex> uplist(0,n);
  svector<tvertex> downlist(0,n);
  int nup,ndown;
  int ntry,nsucc;
  ntry=nsucc=0;
  for (i=0;i<ei;i++)
      {b=eorder[i].firsttbrin();
      e0=G0.NewEdge(G.vin[b],orig[-b]);
      ecolor[e0]=Cyan;
#ifdef TDEBUG
//       DrawGraph(G0);
//       Twait("next");
#endif
      if (!G0.TestPlanar())
          { ntry++;
          nup=UpperList(G,orig[-b],y,uplist);
          for (j=0; j<nup; j++)
              {G0.MoveBrin(e0.secondtbrin(),uplist[j]);
              if (G0.TestPlanar())
                  { G.MoveBrin(-b,uplist[j]);
                  ecolor[e0]=Green;
#ifdef TDEBUG
//                   DrawGraph(G0);
//                   Twait("Ok");
#endif
                  nsucc++; 
                  break;
                  }
              }
	  if (j==nup)
	    {G0.MoveBrin(e0.secondtbrin(),orig[-b]);
	    ndown=LowerList(G,orig[b],downlist);
	    for (j=0; j<ndown; j++)
	      {G0.MoveBrin(e0.firsttbrin(),downlist[j]);
	      if (G0.TestPlanar())
		{ G.MoveBrin(b,downlist[j]);
		ecolor[e0]=Blue;
#ifdef TDEBUG
// 		DrawGraph(G0);
// 		Twait("Ok");
#endif
		nsucc++;
		break;
		}
	      }
	    if (j==ndown) 
	      {          G0.MoveBrin(e0.firsttbrin(),orig[b]);
		ecolor[e0]=Red;
#ifdef TDEBUG
// 		DrawGraph(G0);
// 		Twait("bad");
#endif
	      }
	    }
	  }
      }
  return ntry-nsucc;
  }
int BipPlanarize(TopologicalGraph &G, svector<tbrin>
                  &topin, svector<tvertex> &order,
                  svector<tvertex> &orig, tbrin bst) 
  {int n=G.nv();
  int m=G.ne();
  tedge e;
  tvertex v;
  orig=G.vin;
  int i;

  if (G.TestPlanar())
      return 0;
  
  // invert order
  svector<int> rank(0,n);
  for (i=0; i<n; i++)
    rank[order[i]]=i;

  // mark tree
  svector<bool> mark(0,m); mark.clear();
  for (v=1; v <= n; v++)
      if (topin[v]!=0)
          { e = topin[v].GetEdge();
          mark[e]=true;
          }
  
  // try to planarize G
  
  tvertex s=order[0];
  tvertex t=order[n-1];
  tbrin b,b0;
  
  // Sort cotree edges
  svector<tedge> eorder(0,m-n);
  int ei=0;
  int j;
  svector<tedge> link(0,m);
  svector<tedge> top(0,n); top.clear();
  tvertex w;
  for (i=n-1; i>=0; i--)
      { v = order[i];
      b0 = b = G.pbrin[v];
      do 
          {if (b.out() && !mark[b.GetEdge()])
              { e=b.GetEdge();
              w = orig[-b];
              link[e]=top[w];
              top[w]=e;
              }
          b=G.cir[b];
          } while (b!=b0);
      }
  for (i=n-1; i>=0; i--)
      { v=order[i];
      e = top[v];
      while (e!=0)
          { eorder[ei++]=e;
          e=link[e];
          }
      }
  
  // mark st-edge for non-deletion
  mark[bst.GetEdge()]=true;
  
  GraphContainer GC(G.Container());
  TopologicalGraph G0(GC);
#ifdef TDEBUG
  Prop<int> ewidth(G0.Set(tedge()),PROP_WIDTH,1);
  Prop<short> ecolor(G0.Set(tedge()),PROP_COLOR);
  if (debug())
      for (e=m; e>0; e--)
          ecolor[e]=Black;
#endif


  // erase all but the tree and (s,t) edge
  for (e=m; e>0; e--)
      if (!mark[e])
	G0.DeleteEdge(e);
  // reinsert edges in the good order
  tedge e0;
  int ntry,nsucc;
  ntry=nsucc=0;
  for (i=0;i<ei;i++)
      {b=eorder[i].firsttbrin();
      e0=G0.NewEdge(G.vin[b],orig[-b]);
#ifdef TDEBUG
//       if (debug())
//           {
//           ecolor[e0]=Cyan;
//           DrawGraph(G0);
//           Twait("next");
//           }
#endif
      if (!G0.TestPlanar())
          { ntry++;
          for (j=rank[orig[-b]]+1; j<n; j++)
              {G0.MoveBrin(e0.secondtbrin(),order[j]);
              if (G0.TestPlanar())
                  { G.MoveBrin(-b,order[j]);
#ifdef TDEBUG
//                   if (debug())
//                       {
//                       ecolor[e0]=Green;
//                       DrawGraph(G0);
//                       Twait("Ok");
//                       }
#endif
                  nsucc++; 
                  break;
                  }
              }
          if (j==n) 
              {
              G0.MoveBrin(e0.firsttbrin(),orig[b]);
#ifdef TDEBUG
//               if (debug())
//                   {
//                   ecolor[e0]=Red;
//                   DrawGraph(G0);
//                   Twait("bad");
//                   }
#endif
              }
          }
      }
  return ntry-nsucc;
  }
