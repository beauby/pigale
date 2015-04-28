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
#include <TAXI/Tmessage.h>
#include <TAXI/graphs.h>
#include <TAXI/bicon.h>
#include <TAXI/color.h>

int TopologicalGraph::DFS(svector<tvertex> &nvin,svector<tbrin> &tb,svector<int> &dfsnum,tbrin b0) 
  {cir[0] = b0; acir[0] = acir[b0]; cir[acir[b0]] = 0;
  const svector<tbrin> &rcir = cir;
  int ret=me().GDFS(rcir,nvin,tb,dfsnum);
  cir[0] = 0;  cir[acir[0]] = b0; acir[0] = 0; // restitute cir
  return ret;
  }
int TopologicalGraph:: DFS(svector<tvertex> &nvin,tbrin b0)
  {int n = nv();
  svector<tbrin> tb(0,n);      tb.SetName("DFS:tb");
  svector<int> dfsnum(0,n);    dfsnum.SetName("DFS:dfsnum");
  return DFS(nvin,tb,dfsnum,b0);
  }
int Graph::GDFSRenum(const svector<tbrin> &cir, svector<tvertex> &nvin)
  {if(debug())DebugPrintf("Executing Graph:GDFSRenum");
  Prop<int> iel (PE(),PROP_LABEL);
  
  Prop<bool> IsTree(Set(tedge()),PROP_ISTREE_LR); IsTree.SetName("IsTree");
  IsTree.clear();
  tvertex v;
  tbrin b0=cir[0];
  int n = nv();
  int m = ne();
  svector<tbrin> tb(0,n); tb.clear(); tb.SetName("GDFSRenu:tb");
  svector<int> dfsnum(0,n);  dfsnum.SetName("GDFSRenu:dfsnum");
  assert(m>=1); // pour les cas particuliers
  //nvin.clear();
  nvin[0]=0;
  tbrin b=b0;
  tedge y=1;
  tedge z=m;
  tvertex w;
  v = vin[b0];
  tb[v]= b0;
  dfsnum[v]=1;
  do
      {
      w=vin[-b];
      if(tb[w]!=0)            // w deja connu ?
          {if (b==tb[v])    // on descend sur l'abre ?
              {
              b.cross();
              v=w;
              }
          else if (dfsnum[v]<dfsnum[w]) // coarbre bas ?
              {iel[z]=b();
              nvin[z.firsttbrin()]=dfsnum[v];
              nvin[z.secondtbrin()]=dfsnum[w];
              z--;
              }
          }
      else                    // arbre bas ?
          {if (w==0) break;
          iel[y]=b();
          IsTree[b.GetEdge()] = true;
          b.cross();
          tb[w]=b;
          nvin[y.firsttbrin()]=dfsnum[v];
          y=nvin[y.secondtbrin()]()=dfsnum[w]=y()+1;
          v = w;
          }
      b = cir[b];
      } while(1);

  if (y != n || z != n-1)
      {DebugPrintf("GDFSRenum z=%d y=%d n=%d",z(),y(),n); return 0;}

  return 1;
  }

// If v is a vertex of the original graph,
// tb[v] is the tree brin in (or out if v is the root)
// dfsnum[v] is the dfs numbering of v (i.e. the corresponding vertex
// in nvin)
int Graph::GDFS(const svector<tbrin> &cir, svector<tvertex>&nvin,
                svector<tbrin> &tb, svector<int> &dfsnum) 
  {if(debug())DebugPrintf("Executing Graph:GDFS");
  int n = nv();
  int m = ne();
  if(m < 1)return 0;
  tvertex v;
  tbrin b0=cir[0];

  tb.clear();
  nvin[0]=0;
  tbrin b=b0;
  tedge y = 1;
  tedge z = m;
  tvertex w;
  tvertex newv = 1;  
  v = vin[b0];
  tb[v] = (tbrin)b0;
  dfsnum[v] = 1;
  //int i;
  //for(i = 1;i <= m ;i++) DebugPrintf("GDFS input:%d %d",vin[i](),vin[-i]());
  //DebugPrintf("N=%d  M=%d",n,m);
  do
      {w=vin[-b];            // w current vertex  
      if (tb[w]!=0)         // w known
          {if (b==tb[v])    // backtraking on the tree
              {b.cross();
              v = w; 	
              newv = dfsnum[v];
              }
          else if(newv<dfsnum[w]) // creating cotree edges
              {
              nvin[-z]=dfsnum[w];
              nvin[z--]=newv;
              }
          }
      else                    // creating tree edges
          {if(w==0) break;
          b.cross();          // opposite half edge
          tb[w] = b;
          nvin[y] = newv;       // low vertex in DFS numerotation
	  newv() = nvin[-y]() = dfsnum[w] = y() + 1;
          ++y(); v = w;
          }
      b = cir[b];
      } while(1);

  //for(i = 1;i < y() ;i++) DebugPrintf("%d %d",nvin[i](),nvin[-i]());
  if(debug())DebugPrintf("END Graph:GDFS");
  if(y != n) return 0; // graph not connected
  else return 1;
  }

int bicon(int n,int m,const svector<tvertex> &nvin, _Bicon &Bicon,svector<tvertex> &low)
  {if(debug())DebugPrintf("Executing DFS:bicon");
  int nbre_fine = 0;
  tvertex nfrom,nto,nformer;
  tedge z;
  tvertex nv;

  low.clear(); low.SetName("bicon:low");
  low[1]=1;

  for (z=n;z<=m;z++)
      {nfrom = nvin[z.firsttbrin()];
      nto = nvin[z.secondtbrin()];       
      if (!low[nfrom]) low[nfrom]=nfrom;
      nformer=0;
      while(!low[nto])
          {Bicon.status[nto-1]=PROP_TSTATUS_THIN;
          ++nbre_fine;
          low[nto]=nfrom; nformer=nto; nto=nvin[nto-1];
          }
      if (nto==nfrom)
          {Bicon.status[nformer-1]=PROP_TSTATUS_LEAF;
          nbre_fine--;
          }
      else if (low[nto]!=nfrom)
          while(Bicon.status[nto-1]==PROP_TSTATUS_THIN)
              {nformer = nto;
              nto=nvin[nto-1];
              if (nto==nfrom) break;
              Bicon.status[nformer-1]=PROP_TSTATUS_THICK;
              }
      }
  // initialization of unintialized lows
  for (nv=2; nv<=n;nv++)
      if (!low[nv]) {low[nv]=nv;}
  if (Bicon.status[1]==PROP_TSTATUS_LEAF)
      {tvertex v = n;
      tbrin b;
      do {b = v()-1;
      v = nvin[b];
      } while (v!=1);
      if (b==tbrin(1))
          {Bicon.status[1]=PROP_TSTATUS_THIN;
          nbre_fine++;
          }
      }
  return (nbre_fine == n-1);
  }

// creates pbrin and cir; and inserts brin 0 befor brin b0
void Graph::PrepDFS(svector<tbrin> &cir,tbrin b0)
  {if(debug())DebugPrintf("Executing Graph:PrepDFS");
  tbrin b;
  tvertex v;
  int i;
  int n = nv();
  int m = ne();
  svector<tbrin> pb(0,n); pb.clear(); pb.SetName("DFS:PrepDFS:pb");
  svector<tbrin> db(0,n); db.SetName("DFS:PrepDFS:db");

  for (i=1; i<=m; i++)
      {
      v = vin[i];
      if (pb[v]!=0) { db[v]=cir[db[v]]=i;}
      else {pb[v]=db[v]=i;}
      v = vin[-i];
      if (pb[v]!=0) { db[v]=cir[db[v]]=-i;}
      else {pb[v]=db[v]=-i;}
      }

  for (v = 1; v <= n; v++)
      cir[db[v]] = pb[v];

  cir[db[vin[b0]]]=0;
  cir[0]=b0;
  }

int TopologicalGraph::DFSRenum(svector<tvertex> &nvin, svector<tedge> &ie, tbrin b0)
  {if(debug())DebugPrintf("Executing TopologicalGraph:GDFSRenum");
  int n = nv();
  int m = ne();
  if(m < 1)return 0;
  tvertex v;
  cir[0] = b0; acir[0] = acir[b0]; cir[acir[b0]] = 0;
  svector<tbrin> tb(0,n); tb.clear(); tb.SetName("DFS:DFSRenum:tb");
  svector<int> dfsnum(0,n); dfsnum.SetName("DFS:DFSRenum:dfsnum");

  nvin[0]=0;
  tbrin b=b0;
  tedge y=1;
  tedge z=m;
  tvertex w;
  v = vin[b0];
  tb[v]=(tbrin)b0;
  dfsnum[v]=1;
  do
      {w=vin[-b];
      if (tb[w]!=0)            // w deja connu ?
          {if (b==tb[v])    // on descend sur l'abre ?
              { b.cross();
              v=w;
              }
          else if (dfsnum[v]<dfsnum[w]) // coarbre bas ?
              {nvin[z.firsttbrin()]=dfsnum[v];
              nvin[z.secondtbrin()]=dfsnum[w];
              ie[z]=b.GetEdge();
              z--;
              }
          }
      else                    // arbre bas ?
          {if (w==0) break;
          ie[y]=b.GetEdge();
          b.cross();
          tb[w]=b;
          nvin[y.firsttbrin()]=dfsnum[v];
          y=nvin[y.secondtbrin()]()=dfsnum[w]=y()+1;
          v = w;
          }
      b = cir[b];
      } while(1);

  cir[0] = 0; cir[acir[0]] = b0; acir[0] = 0;
  if(y != n) return 1;
  else return 0;
  }
// DFSTree (priority DFS)
// Prerequisit: cir is sorted by descending priority. Max -> pbrin
// Post: pbrin is incoming tree (except at v0)
//       PROP_ISTREE is computed
void DFSTree(TopologicalGraph &G, tvertex v0)
{if(debug())DebugPrintf("Executing TopologicalGraph:DFSTree");
  int n=G.nv();
  Prop<bool> istree(G.Set(tedge()),PROP_ISTREE); istree.clear();
  svector<bool> seen(0,n); seen.clear();
  tbrin b0 = G.pbrin[v0];
  G.cir[0] = b0; G.acir[0] = G.acir[b0]; G.cir[G.acir[b0]] = 0;
  // G.acir[b0] = 0; is not necessary
  G.vin[0] = 0;
  G.pbrin[v0] = 0;
  tvertex v=v0;
  tbrin b=0;
  seen[v0]=true;
  do {
    b=G.cir[b];
    tvertex w=G.vin[-b];
    if (b==G.pbrin[v]) // backtrack
      {b = -b;
      v=w;
      }
    else if (!seen[w]) // tree
      {b = -b;
      v = w;
      seen[v]=true;
      G.MoveBrinToFirst(b);
      istree[b.GetEdge()]=true;
      }
  } while (b!=0);
  G.cir[0] = 0; G.cir[G.acir[0]] = b0; G.acir[0] = 0;
  G.pbrin[v0]=b0;
}

// Find a path starting with b0 and ending at a marked vertex.
bool FindPath(TopologicalGraph &G, svector<bool> &vmark, tbrin b0, 
	      svector<tbrin> &Lpath, int &Llen)
{
  tvertex v0=G.vin[b0];
  int n=G.nv();
  svector<bool> seen(0,n); seen.clear();
  svector<tbrin> pb(0,n); pb.clear();
  tvertex v=v0;
  tbrin b=b0;
  seen[v0]=true;
  do {
    tvertex w=G.vin[-b];
    if (b==pb[v]) // backtrack
      {b = -b;
      v=w;
      }
    else if (!seen[w]) // tree
      {b = -b;
      v = w;
      seen[v]=true;
      pb[v]=b;
      if (vmark[v]) // end of the path
	break;
      }
  } while ((b=G.cir[b])!=b0);
  if (b==b0)
    return false; // no path found
  Llen=0;
  while (v!=v0)
    {Lpath[Llen++]=pb[v];
    v=G.vin[-pb[v]];
    }
  return true;
}
