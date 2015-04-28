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


#include <TAXI/Tstring.h>
#include <TAXI/graphs.h>
#include <TAXI/Tmessage.h>
int ComputeEClasses(TopologicalGraph &G);
GraphContainer & QuotientGraph(TopologicalGraph &G, bool geom)
{ 
  int n=G.nv();
  int m=G.ne();
  Prop1<int> ncv(G.Set(),PROP_NCV);
  if (! G.Set(tedge()).exist(PROP_NCE))
      {int hastype = G.Set(tedge()).exist(PROP_TYPE);
      Prop<int> colore(G.Set(tedge()),PROP_TYPE);
      if (!hastype) colore.clear();
      ComputeEClasses(G);
      if (!hastype) G.Set(tedge()).erase(PROP_TYPE);
      }
  Prop1<int> nce(G.Set(),PROP_NCE);
  Prop<tvertex> classv(G.Set(tvertex()),PROP_CLASSV);
  Prop<tedge> classe(G.Set(tedge()),PROP_CLASSE);
  int nclassv = ncv();
  int nclasse = nce();
  GraphContainer &QuotientC = *new GraphContainer();
  QuotientC.setsize(nclassv,nclasse);
  // Representative Election
  Prop<tvertex> Repv(QuotientC.Set(tvertex()),PROP_REPRESENTATIVEV);
  Prop<tedge> Repe(QuotientC.Set(tedge()),PROP_REPRESENTATIVEE);
  Repv.clear();
  Repe.clear();
  int i;
  tvertex v;
  tedge e;
  // remark: vertex 1 is always the representative of its class !
  for (i=0,v=1; v<=n && i<nclassv; ++v)
      if (Repv[classv[v]]==0)
          { Repv[classv[v]]=v;
          ++i;
          }
  for (i=0,e=1; e<=m && i<nclasse; ++e)
      if (Repe[classe[e]]==0)
          { Repe[classe[e]]=e;
          ++i;
          }
  Prop<tvertex> cvin(QuotientC.Set(tbrin()),PROP_VIN);
  tedge ec;
  for (ec=1; ec<= nclasse; ++ec)
      { e=Repe[ec];
      cvin[ec.firsttbrin()]=classv[G.vin[e.firsttbrin()]];
      cvin[ec.secondtbrin()]=classv[G.vin[e.secondtbrin()]];
      }
  // If G is a hypergraph, compute related properties on minor.
  if (G.Set().exist(PROP_HYPERGRAPH))
      { int nhv=0;
      int nhe=0;
      Prop<bool> nise(QuotientC.Set(tvertex()),PROP_HYPEREDGE);
      Prop<bool> ise(G.Set(tvertex()),PROP_HYPEREDGE);
      
      for (v=1; v<=nclassv; v++)
          {if ((nise[v]=ise[Repv[v]])==true)
              ++nhe;
          else
              ++nhv;
          }
#ifdef TDEBUG
      for (v=1; v<=n; v++)
          {if (nise[classv[v]]!=ise[v])
              exit(1);
          }
#endif
      Prop1<int> hnv(QuotientC.Set(),PROP_NV); hnv()=nhv;
      Prop1<int> hne(QuotientC.Set(),PROP_NE); hne()=nhv;
      Prop1<int> is_bipartite(QuotientC.Set(),PROP_BIPARTITE);
      Prop1<int> is_hypergraph(QuotientC.Set(),PROP_HYPERGRAPH);
      }
  if (geom)
      {
      GeometricGraph GG(G);
      TopologicalGraph Q(QuotientC);
      GeometricGraph GQ(Q);
      for (v=1; v<=GQ.nv(); ++v)
          {
          GQ.vcoord[v]=GG.vcoord[Repv[v]];
          GQ.vcolor[v]=GG.vcolor[Repv[v]];
          GQ.vlabel[v]=GG.vlabel[Repv[v]];
          }
      for (e=1; e<=GQ.ne(); ++e)
          {
          GQ.ewidth[e]=GG.ewidth[Repe[e]];
          GQ.ecolor[e]=GG.ecolor[Repe[e]];
          GQ.elabel[e]=GG.elabel[Repe[e]];
          }
      }
  return QuotientC;
}

GraphContainer * ReducedGraph(TopologicalGraph &G)
{ 
 int n=G.nv();
 GraphContainer &Reduced = *new GraphContainer;
 Prop1<tstring> title(G.Set(),PROP_TITRE);
 Prop1<tstring> rtitle(Reduced.Set(),PROP_TITRE);
 rtitle() = "Reduced-"+title();
 Prop<tvertex> vred(G.Set(tvertex()),PROP_REDUCED);  
 // Phase 1:
 //     We renumber the vertices according to reduced classes.
 //     indices are consecutives!!!
 //
 // At v, color k -> (k,-) or (-,k) or (k,++j), where 1st ind ->
 // adjacent, 2nd -> non-adjacent and - means none exist.
 // A representative is "elected" for each color.
 
 svector<bool> seei(0,n);
 svector<int> numj(0,n);
 svector<bool> done(1,n); done.clear();
 svector<int> color(0,n);
 svector<tvertex> representative(0,n);
 svector<tedge> erepresentative(0,G.ne());
 representative[1]=1;
 int j=1;
 tvertex v,w;
 tedge e;
 tbrin b,b0;
 int colw,ncol,colv;
 svector<int> index(0,G.nv());
 for (v=1; v<=n; ++v)
     color[v]=1;
 for (v=1; v<=n; ++v)
     { seei.clear();
     b=b0=G.pbrin[v];
     do
         {w=G.vin[-b];
         colw=color[w];
         if (!seei[colw])
             { seei[colw]=true;
             numj[colw]=0;
             representative[colw]=w;
             }
         done[w]=true;
         b=G.cir[b];
         } while(b!=b0);
    for (w=1; w<=n; ++w)
        {if (done[w])
          done[w]=false;
        else
            {colw=color[w];
            if (seei[colw])
                if ((ncol=numj[colw])!=0)
                    {color[w]=ncol;}
                else
                    {color[w]=numj[colw]=++j;
                    representative[j]=w;
                    }
            else
                {representative[colw]=w;
                }
            }  
        }
     }
 // Vertex 1 must be colored 1 and must be the representative
 // of 1!
 if (representative[1]!=1)
     {if (color[1]!=1)
         {int oc=color[1];
         tvertex ore=representative[1];
         for (v=1; v<=n; v++)
             if (color[v]==oc) color[v]=1;
             else if (color[v]==1) color[v]=oc;
         representative[oc]=ore;
         }
     representative[1]=1;
     }
 
 // Phase 2: build the reduced graph. We consider only the edges
 // between representatives.
 tbrin nb=0;
 svector<tvertex> rv(-G.ne(),G.ne());
 rv[0]=0;
 for (colv=2; colv<=j; colv++)
     {v = representative[colv];
     b=b0=G.pbrin[v];
     do
         {w=G.vin[-b];
         colw = color[w];
         if (colw<colv && w==representative[colw])
             {
             ++nb;
             rv[nb]=colv;
             rv[-nb]=colw;
             erepresentative[nb]=b.GetEdge();
              seei[colv]=true;
             }
         b=G.cir[b];
         } while(b!=b0);
     }
 // Update vred (it is color)
 for (v=1; v<=n; v++) vred[v]=color[v];
 Reduced.setsize(j,nb());
 Tprintf("Reduction of n-m from %d-%d to %d-%d",n,G.ne(),j,nb());
 Prop<tvertex> rvin(Reduced.Set(tbrin()),PROP_VIN);
  for (b=-nb; b<=nb;b++) rvin[b]=rv[b];
  Prop<tvertex> repv(Reduced.Set(tvertex()),PROP_INITIALV);
  for (v=1; v<=j; v++)
      repv[v]=representative[v];
  Prop<tedge> repe(Reduced.Set(tedge()),PROP_INITIALE);
  for (e=1; e<=nb(); e++)
      repe[e]=erepresentative[e];
  return &Reduced;
}
