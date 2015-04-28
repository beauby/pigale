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

#ifndef _NDFS_H_
#define _NDFS_H_

#include <TAXI/Tbase.h>
#include <TAXI/color.h>
#include <TAXI/graphs.h>
#include <TAXI/Tmessage.h>
#define PRIVATE
#include <TAXI/lralgo.h>
#undef PRIVATE
#include <TAXI/Tdebug.h>


// _DFS structure to handle DFS of simple connected graphs

struct _DFS
{
    TopologicalGraph &G;
    int n;
    int m;
    svector<tvertex> nvin;
    svector<tedge> elow;
    svector<tvertex> low;
    svector<tbrin> _ib;
    // svector<tvertex> iv;
    svector<int> DualOrder;
    svector<int> status;

    _DFS(TopologicalGraph &GG) : G(GG), n(GG.nv()), m(GG.ne()),
    nvin(-m,m), elow(0,n), low(0,n), _ib(0,m), // iv(0,n), 
    DualOrder(0,n), status(0,n)
        { init();
        }
    ~_DFS() {}

    void init();
  tbrin ib(tbrin b) 
    {if (b()>=0) return _ib[b](); else return -_ib[-b]();}
  tedge ie(tedge e)
    {return _ib[e].GetEdge();}
  tvertex iv(tvertex v)
    {if (v==0) return 0;
    else if (v==1) return G.vin[-_ib[-1]];
    else return G.vin[-_ib[v()-1]];
    }
    int DoDFS(tbrin b0=1);
    int bicon();
    void DFSDualOrder();
    tbrin treein(tvertex v) {return v()-1;}
    tvertex treetarget(tedge e) {return e()+1;}
    tvertex father(tvertex v) {return nvin[v()-1];}
    bool SmallerThan(tvertex v, tvertex w)
        { return (v<w) && (DualOrder[v]<DualOrder[w]); }
    bool SmallerEqual(tvertex v, tvertex w)
        { return (v<=w) && (DualOrder[v]<=DualOrder[w]); }
    bool Comparable(tvertex v,tvertex w)
        { if (v<w) return (DualOrder[v]<DualOrder[w]);
        else return (DualOrder[v]>=DualOrder[w]);
        }
    bool ComparableDifferent(tvertex v,tvertex w)
        { if (v<w) return (DualOrder[v]<DualOrder[w]);
        else return (DualOrder[v]>DualOrder[w]);
        }
    void SwapInOrder(tedge &e, tedge &f)
        {tvertex v1=nvin[e];
        tvertex v2=nvin[f];
        tedge g=e;
        if (v1<v2) ;
        else if (v1>v2)
            { e=f; f=g;}
        else if (nvin[-e]<nvin[-f])
            {e=f; f=g;}
        }
    bool OrdInterlaced(tedge e, tedge f) // e,f : cotree edges in order
        {
        return ((nvin[e]!=nvin[f]) && (nvin[f]<nvin[-e]) &&!SmallerEqual(nvin[-f],nvin[-e]));
        }
    bool Interlaced(tedge e, tedge f) // e,f : cotree edges
        { tvertex v1=nvin[e];
        tvertex v2=nvin[f];
        if (v1==v2) return false;
        if (v1<v2) return ((nvin[f]<nvin[-e]) &&!SmallerEqual(nvin[-f],nvin[-e]));
        else return ((nvin[e]<nvin[-f]) && !SmallerEqual(nvin[-e],nvin[-f]));
        }
    bool OrdChainInterlaced(tedge e, tedge f)
        { return ((nvin[e]!=nvin[f]) && (nvin[f]<nvin[-e]) 
                  && SmallerThan(nvin[-e],nvin[-f]));
        }
    tvertex infimum(tvertex v, tvertex w);
    tedge HighestCove(tvertex v1, tvertex v2);
    tvertex cover(tvertex from, tvertex to);
    void LralgoSort(_LrSort &LrSort);
    int Lralgo( _LrSort &LrSort, _FastHist &Hist);
};


class _Kuratowski : public _DFS
{
    Prop<int> ewidth;
    Prop<short> ecolor;
    Prop<short> vcolor;
    Prop<int> elabel;
    Prop<bool> mark;
    svector<tvertex> ancestor;
    svector<tvertex> ancestor_son;
    tedge e1,e2,np;
    tedge down_edge;
    tedge down_edge_equal;
    tvertex down_vertex;
    bool isplanar;
    bool EgaliteBasse;
    tvertex fils1, fils2, branch;
    tvertex npattach;
    tvertex l1,l2,lp,t1,t2;
    bool Success;
    int RetVal;
    short lastc;
    tedge laste;


public:

    _Kuratowski(TopologicalGraph &G) : _DFS(G), ewidth(G.Set(tedge()),PROP_WIDTH,1),
    ecolor(G.Set(tedge()),PROP_COLOR,1), vcolor(G.Set(tvertex()),PROP_COLOR,Yellow),
    elabel(G.Set(tedge()),PROP_LABEL),mark(G.Set(tedge()),PROP_MARK),
    ancestor(0,n), ancestor_son(0,n), Success(false),RetVal(0),laste(0)
        {Compute();}

    ~_Kuratowski() {}

public:
    /*
    void Flash(tedge e, short c)
        { if (!debug()) return;
        short ec = ecolor[ie(e)];
        ecolor[ie(e)] = c;
        DrawGraph();
        ecolor[ie(e)] = ec;
        }
    void FlashBest(tedge e, short c)
        { if (!debug()) return; 
        short ec = ecolor[ie(e)];
        ecolor[ie(e)] = c;
        DrawGraph();
        ecolor[ie(e)] = ec;
        }
        */
    void Check();
    bool KuraFound() { return Success;}
    bool IsCotreeCritical();
    int GetRetVal(){return RetVal;}
protected :
    void Compute();
    int KTestNonPlanar(bool show=false);
    void ComputeAncestors();
    void ComputeImportantVertices()
        {l1 = nvin[e1]; t1=nvin[-e1];
        l2 = nvin[e2]; t2=nvin[-e2];
        lp = nvin[np];
        branch = infimum(t1,t2);
        npattach = infimum(branch,nvin[-np]);
        fils1 = cover(branch,t1);
        fils2 = cover(branch,t2);
        }
    void ComputeDownEdge();
    tedge FindInterlaced(tedge e, tvertex lv, tvertex tv);
    tedge FindLastInterlaced(tedge e,tvertex lv,tvertex tv);

    bool IsDownEdgeOk(tedge e)
        { return ((nvin[down_edge]<l1)&&(ancestor[nvin[-e]] > down_vertex)); }
   bool IsDownEdgeEqualOk(tedge e)
        { return ((down_edge_equal!=0) &&(ancestor[nvin[-e]] > branch)); }
    bool IsNPEdgeOk(tedge e)
      { return (nvin[e] < ancestor[nvin[-np]]) ? true:false; }

    tedge FindLowEdge(tedge e)
        {tvertex vason = ancestor_son[nvin[-e]];
        if ((vason!=0) && (low[vason] < l1))
            return elow[vason];
        else 
            return 0;
        }
    void Keep(tedge e, short c)
        { ecolor[ie(e)] = c;
        mark[ie(e)] = true;
        }
    void UnKeep(tedge e, short c)
        { ecolor[ie(e)] = c;
        mark[ie(e)] = false;
        }
};
    
#endif
