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
#include <TAXI/bicon.h>
#include <TAXI/Tmessage.h>
void DistFromRoot(TopologicalGraph &G, tvertex v0, svector<int> &dist);

bool MarkBloc(TopologicalGraph &G,
              tbrin b0, svector<bool> &mark, bool marked) 
  {
  int m=G.ne();
  if (m==0) return false;
  int n=G.nv();

  svector<tvertex> nvin(-m,m);
  svector<tvertex> low(0,n);
  svector<tbrin> tb(0,n);
  svector<int> dfsnum(0,n);

  // perform a DFS
  if (!G.DFS(nvin,tb,dfsnum,b0)) // not connected
      return false;
  // compute the low
  _Bicon Bicon(n);
  
  tvertex v;
  
  int ret = bicon(n,m,nvin,Bicon,low);
  if (ret)
      for (v=1;v<=n;v++)
          mark[v]=marked;
  else
      {
          // mark the (new) vertices in the bloc of b0
          svector<bool> nmark(0,n); nmark.clear();
          
          nmark[1]=true;
          nmark[2]=true;
          for (tvertex nv=3; nv<=n; nv++)
              {if (nvin[nv()-1]==1) break; // 1=articulation.
              if (!(low[nv]<nvin[nv()-1]&& nmark[nvin[nv()-1]]))
                  continue;
              nmark[nv]=true;
              }
          // copy the marks
          for (v=1; v<=n;v++)
              {        
                  if (nmark[dfsnum[v]]) mark[v]=marked;
                  else mark[v]=!marked;
              }          
      }
  return true;
  }

// Mark and orient paths between two points.
// potentials are returned in PROP_TMP.

void MarkPaths(TopologicalGraph &G,
               tvertex s, tvertex t, int depth,
               svector<bool> &mark, bool marked) 
  {int n=G.nv();
  int m=G.ne();
  tvertex v,w;
  
  svector<int> dists(0,n);
  DistFromRoot(G,s,dists);
  svector<int> distt(0,n);
  DistFromRoot(G,t,distt);
  for (v=1; v<=n; v++)
      if (dists[v]+distt[v]>depth) mark[v]=!marked;
      else mark[v]=marked;
  
  G.Set(tvertex()).erase(PROP_TMP);
  Prop<int> f(G.Set(tvertex()),PROP_TMP);
  for (v=1; v<=n; v++)
    if (mark[v])
      f[v]=dists[v]-distt[v];
  --f[s];
  ++f[t];
  Prop<bool> Oriented(G.Set(tedge()),PROP_ORIENTED,true);
  for (tbrin b=1; b<=m; b++) // outgoing brins -> (v,w)
      {v=G.vin[b];
      w=G.vin[-b];
      if (mark[v] && mark[w] && (f[v]!=f[w]))
          {
              Oriented[b.GetEdge()]=true;
              if (f[v]>f[w])
                  G.ReverseEdge(b.GetEdge());
          }
      }
  G.FixOrientation();  
  }
// Prerequisit:
//   If PROP_CLASSV exist, should get consecutive values 
//   from 1 to ..
//   PROP_TYPE values should go from 1 to ... (not too big)
// Post:
//   PROP_CLASSV -> vertex class.
//   return -> # of vertex classes
void SortCir(TopologicalGraph &G, svector<tbrin> &ncir, svector<tbrin>
	     &npbrin, svector<int> &colore, int ncolore);

int MarkVClasses(TopologicalGraph &G)
{ tvertex v,w;
  tbrin b,b0,bb;
  tedge e;
  int n=G.nv();
  int m=G.ne();

  // Init V classes

  int nclassv=0;
  int ExistVClass = G.Set(tvertex()).exist(PROP_CLASSV);
  Prop<tvertex> classv(G.Set(tvertex()),PROP_CLASSV);
  classv[0]=0;
  if (!ExistVClass)
    { Fill(classv.vector(),tvertex(1));
    nclassv=1;
    }
  else
    {// compute number of colors
      nclassv = MaxElement(classv.vector())();
    }

  Prop<int> colore(G.Set(tedge()),PROP_TYPE);
  int ncolore=MaxElement(colore.vector());
  svector<tbrin> ncir(-m,m);
  svector<tbrin> npbrin(0,n);
  SortCir(G,ncir,npbrin,colore.vector(),ncolore);

  // Algorithm:
  //   For each vertex
  //      color   -> current edge color
  //      done[w] -> vertex w has been processed
  //      seen[c] -> class c has been reached for current vertex
  //      newc[c] -> new class for neighbours of class c through
  //                 edge of current color or new class for 
  //                 non-neighbours.

  int color;
  tvertex cw;
  svector<tvertex> newc(0,n);
  svector<bool> done(0,n); done.clear();
  svector<bool> seen(0,n);
  int classlim;
  for (color=1; color <= ncolore; ++color)
      {for (v=1; v<=n; v++)
          { seen.clear();
          classlim=nclassv;
          // Process neighbours first
          b=b0=npbrin[v];
          do
              { if (colore[b.GetEdge()]!=color)
                  continue;
              w = G.vin[-b];
              cw = classv[w];
              if (cw <= classlim)
                  {if (!seen[cw]) // First keeps the color
                      { seen[cw]=true;
                      newc[cw]=cw;
                      }
                  else if (newc[cw]==0) // Not first, but first reach
                      { newc[cw]=classv[w]=++nclassv;
                      }
                  else
                      { classv[w]=newc[cw]; // class already seen
                      }
                  done[w]=true;
                  }
              } while ((b=ncir[b])!=b0);
          // Process non neighbours
          newc.clear();
          for (w=1; w<=n; w++)
              { cw=classv[w];
              if (done[w])
                  {done[w]=false;
                  continue;
                  }
              if (seen[cw])
                  {seen[cw]=false;
                  newc[cw]=classv[w]=++nclassv;
                  }
              else if (newc[cw]!=0)
                  {classv[w]=newc[cw];
                  }	
              }
          }
      }
  Prop1<int> ncv(G.Set(),PROP_NCV);
  ncv()=nclassv;
  return nclassv;
}
// Compute edge classes, with respect to a vertex class partition.
int ComputeEClasses(TopologicalGraph &G)
{
  int m=G.ne();
  tvertex v,w,cv,cw;
  tedge e;
  int nclasse=0;
  Prop<tvertex> classv(G.Set(tvertex()),PROP_CLASSV);
  Prop<tedge> classe(G.Set(tedge()),PROP_CLASSE);
  Prop1<int> ncv(G.Set(),PROP_NCV);
  int nclassv = ncv();
  Prop<int> colore(G.Set(tedge()),PROP_TYPE);
  int ncolore=MaxElement(colore.vector());
  svector<tedge> top(0,nclassv); top.clear();
  svector<tedge> link(0,m); link.clear();
  for (e=1; e<=m; e++)
      { v= G.vin[e.firsttbrin()];
      w=G.vin[e.secondtbrin()];
      if (classv[v]>classv[w])
          {tvertex tmp=v; v=w; w=tmp;}
      link[e]=top[cv=classv[v]];
      top[cv]=e;
      }
  svector<tedge> stop(0,nclassv); stop.clear();
  svector<tedge> slink(0,m); slink.clear();
  for (cv=nclassv; cv>0;--cv)
      {e=top[cv];
      while (e!=0)
          { v= G.vin[e.firsttbrin()];
          w=G.vin[e.secondtbrin()];
          if (classv[v]!=cv)
              {tvertex tmp=v; v=w; w=tmp;}
          slink[e]=stop[cw=classv[w]];
          stop[cw]=e;
          e=link[e];
          }
      }
  svector<tedge> nc(0,ncolore); nc.clear();
  
  for(cw=1; cw<=nclassv; cw++)
      { e=stop[cw];
      if (e==0) continue;
      cv=0;
      do
          { v = G.vin[e.firsttbrin()];
          w=G.vin[e.secondtbrin()];
          if (classv[v]>classv[w])
              {tvertex tmp=v; v=w; w=tmp;}
          if (cv!=classv[v])
              { nc.clear();
              cv=classv[v];
              }
          if (nc[colore[e]]==0)
              {
              nc[colore[e]]=++nclasse;
              }
          classe[e]=nc[colore[e]];
          } while ((e=slink[e])!=0);
      stop[cw]=0;
      }
  Prop1<int> nce(G.Set(),PROP_NCE);
  nce()=nclasse;
  return nclasse;
}
