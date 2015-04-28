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
#include <TAXI/Tsmap.h>




GraphContainer &ExtractBall(TopologicalGraph &G,
                            svector<tvertex>&Root,int depth)
{ GraphContainer *pGC= new GraphContainer;
 Prop1<tstring> title(pGC->Set(),PROP_TITRE);
 title() = "Sub Graph";
 svector<tvertex> queue=Root;
 svector<tvertex> vin(0,0); vin[0]=0;
 svector<int> nlabel(0,0); nlabel[0]=0;
 svector<int> nlabele(0,0); nlabele[0]=0;
 Prop<int> label(G.Set(tvertex()),PROP_LABEL);
 Prop<int> labele(G.Set(tedge()),PROP_LABEL);
 svector<tvertex> nnum(0,0); nnum[0]=0;
 svector<tedge> nnume(0,0); nnume[0]=0;
 smap<tvertex> BFSnum;
 int top;
 int nv=0;
 for (top=0; top<queue.stopi(); top++)
     {BFSnum[queue[top]()]=++nv;
     nlabel(nv)=label[queue[top]];
     nnum(nv)=queue[top];
     }
 int bot=0;
 int ne=0;
 int d=0;
 int first=top;
 tvertex v,w;
 tbrin b,b0;
 while (bot<top)
     {v=queue[bot];
     if (bot==first)
         {first=top;
         if (++d >= depth)
             break;
         }
     b=b0=G.pbrin[v];
     do
         {w=G.vin[-b];
         if (BFSnum.ExistingIndexByKey(w())<0)
             {BFSnum[w()]=++nv;
             queue(top++)=w;
             nlabel(nv)=label[w];
             nnum(nv)=w;
             }
         if (BFSnum[v()]<BFSnum[w()])
             { vin(++ne)=BFSnum[v()];
             vin(-ne)=BFSnum[w()];
             nlabele(ne)=labele[b.GetEdge()];
             nnume(ne)=b.GetEdge();
             }
         b=G.cir[b];
         } while (b!=b0);
     ++bot;
     }
 pGC->setsize(nv,ne);
 pGC->Set(tbrin()).swload(PROP_VIN,vin);
 
 if (G.Set().exist(PROP_HYPERGRAPH))
     {Prop1<int> is_bipartite(pGC->Set(),PROP_BIPARTITE);
     Prop1<int> is_hypergraph(pGC->Set(),PROP_HYPERGRAPH);
     Prop1<int> nhv(pGC->Set(),PROP_NV);
     Prop1<int> nhe(pGC->Set(),PROP_NE);
     Prop<bool> ise(G.Set(tvertex()),PROP_HYPEREDGE);
     Prop<bool> nise(pGC->Set(tvertex()),PROP_HYPEREDGE);
     nhe()=0;
     nhv()=0;
     for (v=1; v<=nv;v++)
         if ((nise[v]=ise[nnum[v]])==true)
             ++nhv();
         else
             ++nhe();
     }
 pGC->Set(tvertex()).swload(PROP_LABEL,nlabel);
 pGC->Set(tedge()).swload(PROP_LABEL,nlabele);
 pGC->Set(tvertex()).swload(PROP_INITIALV,nnum);
 pGC->Set(tedge()).swload(PROP_INITIALE,nnume); 
 return *pGC; 
}
void DistFromRoot(TopologicalGraph &G, tvertex v0, svector<int> &dist)
  {int n=G.nv();
  dist.clear();
  svector<tvertex> queue(0,n);
  int bot=0;
  int top=0;
  tbrin b,b0;
  tvertex v,w;
  
  queue[top++]=v0;
  while (top>bot)
      {
          v=queue[bot++];
          b=b0=G.pbrin[v];
          do
              {
                  w=G.vin[-b];
                  if ((w!=v0)&&(dist[w]==0))
                      {
                          queue[top++]=w;
                          dist[w]=dist[v]+1;
                      }
                  b=G.cir[b];
              } while (b!=b0);   
      }
  }

// How it works:
// 0 -> i1 -> ... -> ik -> 0 : list of available indices
// indices are taken and given at the begining of the list

template <class T>
class queueset {
private:
    svector<T> q;
    svector<int> botq;
    svector<int> topq;
    svector<int> next;
    int maxind;

    int take() 
        {
        int i=next[0];
        next[0]=next[i];
        return i;
        }
    void give_back(int i)
        {
        int j=next[0];
        next[i]=j;
        next[0]=i;
        }
    int take(int num)
        {int ind=take();
        if (botq[num]==0) botq[num]=ind;
        else next[topq[num]]=ind;
        topq[num]=ind;
        return ind;
        }
    void crunch(int num)
        {int i=botq[num];
        if (topq[num]==i) topq[num]=botq[num]=0;
        else botq[num]=next[i];
        give_back(i);
        }
        
public:
    void reset()
        {
        botq.clear();
        topq.clear();
        // everything is available
        for (int i=0; i<maxind; i++)
            next[i]=i+1;
        next[maxind]=0; // end
        }
    bool empty(int i) {return botq[i]==0;}
    queueset(int maxi,int nq):q(0,maxi), botq(0,nq), topq(0,nq), next(0,maxi),maxind(maxi) {reset(); }
    void put (int i, const T& elmt)
        {int ind=take(i);
        q[ind]=elmt;
        }
    bool get (int i, T &elmt)
        { if (!empty(i))
            {
            elmt=q[botq[i]];
            crunch(i);
            return true;
            }
        else return false;
        }
};



// BFS with strong priority to edges
// All the edges with smallest order are processed first, etc.
// orders go from 0 to maxo

void BFSTree(TopologicalGraph &G, tvertex v0, svector<int> &order, int maxo)
{
  int n=G.nv();
  int m=G.ne();
  Prop<bool> istree(G.Set(tedge()),PROP_ISTREE); istree.clear();
  svector<bool> seen(0,n); seen.clear();
  queueset<tbrin> qset(m,maxo);
  tvertex v=v0;
  tbrin b,bb;
  int ord;
  while (v!=0)
    { // mark v
    seen[v]=true;
    // push v incidences on the queues
    b=bb=G.pbrin[v];
    do {
	if (!seen[G.vin[-b]])
        {// push opposite brin
	    ord = order[b.GetEdge()];
        qset.put(ord,-b);
        }
	b=G.cir[b];
    } while (b!=bb);
    // pop tree edge
    v=0;
    ord=0;
    while(ord<=maxo)
        {if (qset.get(ord,b))
            {
            v = G.vin[b];
            if (!seen[v]) // found !
                {istree[b.GetEdge()]=true;
                G.pbrin[v]=b;
                break;
                }
            }
        else ++ord;
        }
    }
}
template <class T>
class queue {
private:
    svector<T> q;
    int botq;
    int topq;

public:
    void reset() {botq=topq=0;}
    bool empty() {return botq==topq;}
    queue(int size):q(0,size)  {reset(); }
    void put (const T& elmt)    {  q[topq++]=elmt;  }
    bool get (T &elmt)
        { if (topq>botq)
            { elmt=q[botq++];
            return true;
            }
        else return false;
        }
};          
        

// BFS which traverses directed edges only.

void BFSDirectedTree(TopologicalGraph &G, tvertex v0)
{
//int n=G.nv();
  int m=G.ne();
  Prop<bool> oriented(G.Set(tedge()),PROP_ORIENTED,false);
  Prop<bool> istree(G.Set(tedge()),PROP_ISTREE); istree.clear();
  Prop<bool> seen(G.Set(tvertex()),PROP_MARK); seen.clear();
  queue<tbrin> q(m);
  tvertex v=v0;
  tbrin b,bb;
  while (v!=0)
    { // mark v
      seen[v]=true;
      // push v incidences on the queues
      b=bb=G.pbrin[v];
      do {
      if (!seen[G.vin[-b]])
          {// push opposite brin
          if (b.out() && oriented[b.GetEdge()])
              {q.put(-b);
              }
          }
      b=G.cir[b];
      } while (b!=bb);
      // pop tree edge
      v=0;
      while(q.get(b))
          { v = G.vin[b];
          if (!seen[v]) // found !
              {istree[b.GetEdge()]=true;
              G.pbrin[v]=b;
              break;
              }
          }
    }
}
// BFS which orrients the edges.

void BFSOrientTree(TopologicalGraph &G, tvertex v0)
{
//int n=G.nv();
  int m=G.ne();
  Prop<bool> oriented(G.Set(tedge()),PROP_ORIENTED,true); 
  Prop<bool> reoriented(G.Set(tedge()),PROP_REORIENTED,false); reoriented.clear();
  Prop<bool> istree(G.Set(tedge()),PROP_ISTREE); istree.clear();
  Prop<bool> seen(G.Set(tvertex()),PROP_MARK); seen.clear();
  queue<tbrin> q(m);
  //svector<tbrin> qlink(-m,m); qlink.clear();
  tvertex v=v0;
  tbrin b,bb;
  tedge e;

  for (e=1;e<=G.ne();++e)
      oriented[e]=true;
  
  while (v!=0)
      { // mark v
      seen[v]=true;
      // push v incidences on the queues
      b=bb=G.pbrin[v];
      do
          {
          if (!seen[G.vin[-b]])
              {// push opposite brin
              if (b.in())
                  reoriented[b.GetEdge()]=true;
              q.put(-b);
              }
          // otherwise, already processed.
          else if (b.out())
              reoriented[b.GetEdge()]=true;
          b=G.cir[b];
          } while (b!=bb);
      // pop tree edges
      v=0;
      while(q.get(b))
          { v = G.vin[b];
          if (!seen[v]) // found !
              {istree[b.GetEdge()]=true;
              G.pbrin[v]=b;
              break;
              }
          else if (b.out()) // forward cotree edge
              reoriented[b.GetEdge()]=true;
          }
      }
}
