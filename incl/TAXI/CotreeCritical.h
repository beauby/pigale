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

#ifndef _NNDFS_H_
#define _NNDFS_H_
#include <TAXI/DFSGraph.h>
#include <TAXI/Tmessage.h>

struct DFSKura : public DFSGraph
{
    Prop<int> ewidth;
    Prop<short> ecolor;
    Prop<short> vcolor;
    Prop<long> elabel;
    tedge e1,e2,np;
    bool isplanar;
    bool EgaliteBasse;

    DFSKura(DFSGraph &DG) : DFSGraph(DG), ewidth(G.Set(tedge()),PROP_WIDTH,1),
        ecolor(G.Set(tedge()),PROP_COLOR,Black), 
        vcolor(G.Set(tvertex()),PROP_COLOR,Yellow),
        elabel(G.Set(tedge()),PROP_LABEL)
        {
        ShrinkToNonPlanar(true);
        }
    /*    
    DFSKura(DFSKura &DK) : DFSGraph(DK), ewidth(G.Set(tedge()),PROP_WIDTH,1),
        ecolor(G.Set(tedge()),PROP_COLOR,1), 
        vcolor(G.Set(tvertex()),PROP_COLOR,yellow),
        elabel(G.Set(tedge()),PROP_LABEL)
        {}
        */
    ~DFSKura() {}
    int ShrinkToNonPlanar(bool show=false);
};


class DFSKura2 : public DFSKura
{
    Prop<bool> mark;
    Prop<int> DualOrder;
    Prop<tvertex> low;
    Prop<tedge> elow;
    svector<tvertex> ancestor;
    svector<tvertex> ancestor_son;
    tedge down_edge;
    tedge down_edge_equal;
    tvertex down_vertex;
    tvertex fils1, fils2, branch;
    tvertex npattach;
    tvertex l1,l2,lp,t1,t2;
    bool Success;
    int RetVal;
    //hub short lastc;
    tedge laste;

public:
    DFSKura2(DFSGraph &DG) : DFSKura(DG),
        mark(Set(tedge()),PROP_MARK),
        DualOrder(Set(tvertex()),PROP_DUALORDER),
        low(Set(tvertex()),PROP_LOW),
        elow(Set(tvertex()),PROP_ELOW),
        ancestor(0,nv()),
        ancestor_son(0,nv()), 
        Success(false),RetVal(0),laste(0)
        {
        if(!isplanar) 
            {bicon();
            DFSDualOrder();
            Compute();
            }
        }
    /*
    DFSKura2(DFSKura &DK) : DFSKura(DK),
        mark(Set(tedge()),PROP_MARK),
        DualOrder(Set(tvertex()),PROP_DUALORDER),
        low(Set(tvertex()),PROP_LOW),
        elow(Set(tvertex()),PROP_ELOW),
        ancestor(0,nv()),
        ancestor_son(0,nv()), 
        Success(false),RetVal(0),laste(0)
        {if (!isplanar) 
            {bicon();
            DFSDualOrder();
            Compute();
            }
        }
        */
    ~DFSKura2() 
        {
        }
    

 public:
    
    void DFSDualOrder();
    int SmallerThan(tvertex v, tvertex w)
        {return (v<w) && (DualOrder[v]<DualOrder[w]); }
    int SmallerEqual(tvertex v, tvertex w)
        {return (v<=w) && (DualOrder[v]<=DualOrder[w]); }
    int Comparable(tvertex v,tvertex w)
        {if (v<w) return (DualOrder[v]<DualOrder[w]);
        else return (DualOrder[v]>=DualOrder[w]);
        }
    int ComparableDifferent(tvertex v,tvertex w)
        {if (v<w) return (DualOrder[v]<DualOrder[w]);
        else return (DualOrder[v]>DualOrder[w]);
        }

    int OrdInterlaced(tedge e, tedge f) // e,f : cotree edges in order
        {return ((nvin[e]!=nvin[f]) && (nvin[f]<nvin[-e])
                 && !SmallerEqual(nvin[-f],nvin[-e]));
        }
    int Interlaced(tedge e, tedge f) // e,f : cotree edges
        { tvertex v1=nvin[e];
        tvertex v2=nvin[f];
        if (v1==v2) return 0;
        if (v1<v2) return ((nvin[f]<nvin[-e]) 
                           && !SmallerEqual(nvin[-f],nvin[-e]));
        else return ((nvin[e]<nvin[-f]) && !SmallerEqual(nvin[-e],nvin[-f]));
        }
    int OrdChainInterlaced(tedge e, tedge f)
        {return ((nvin[e]!=nvin[f]) && (nvin[f]<nvin[-e]) 
                  && SmallerThan(nvin[-e],nvin[-f]));
        }
    tvertex infimum(tvertex v, tvertex w);
    tedge HighestCove(tvertex v1, tvertex v2);
    tvertex cover(tvertex from, tvertex to);
    void ComputeAncestors();
    void Flash(tedge e, short c)
        //{if(!debug()) return;
        {if(e()) return;
        short ec = ecolor[ie(e)];
        ecolor[ie(e)] = c;
        DrawGraph();
        ecolor[ie(e)] = ec;
        }
    void FlashBest(tedge e, short c) 
        //{if(!debug()) return;
        {if(e()) return;
        short ec = ecolor[ie(e)]; 
        ecolor[ie(e)] = c;
        DrawGraph();
        ecolor[ie(e)] = ec; 
        } 
    bool KuraFound() { return Success;} 
    int GetRetVal(){return RetVal;}
    protected :
        void Compute(); 
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
        {return ((nvin[down_edge]<l1)&&(ancestor[nvin[-e]] > down_vertex)); }
    bool IsDownEdgeEqualOk(tedge e)
        {return ((down_edge_equal!=0) &&(ancestor[nvin[-e]] > branch)); }
    bool IsNPEdgeOk(tedge e)
      {return (nvin[e] < ancestor[nvin[-np]]) ? true:false; }
    
    tedge FindLowEdge(tedge e)
        {tvertex vason = ancestor_son[nvin[-e]];
        if((vason!=0) && (low[vason] < l1))
            return elow[vason];
        else 
            return 0;
        }
    void Keep(tedge e, short c)
        {if(debug())ecolor[ie(e)] = c;
        mark[e] = true;
        }
    void UnKeep(tedge e, short c)
        {if(debug()) ecolor[ie(e)] = c;
        mark[e] = false;
        }
};
#endif
