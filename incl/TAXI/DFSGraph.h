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

#ifndef _DFSGRAPH_H_
#define _DFSGRAPH_H_

#include <TAXI/Tbase.h>
#include <TAXI/color.h>
#include <TAXI/graphs.h>
#include <TAXI/Tdebug.h>

// _DFS structure to handle DFS of simple connected graphs

struct DFSGraph : public GraphAccess
{
  GraphAccess &G;
  Prop<tvertex> Gvin;     // vin du graphe initial
  Prop<tvertex> nvin;
  Prop<tbrin> _ib;
  svector<tedge> uptree;
    
  //svector<tvertex> nvin;
  
  
  DFSGraph(GraphContainer &GC,Graph &GG,tbrin b0=1) :
    GraphAccess(GC), G(GG), Gvin(GG.Set(tbrin()),PROP_VIN),
    nvin(Set(tbrin()),PROP_VIN), _ib(Set(tedge()),PROP_INITIALB), uptree(G.nv(),G.ne())
  {setsize(G.nv(),G.ne());
  nvin.SetName("DFSGraph:nvin");
  _ib.SetName("DFSGraph:_ib");
  uptree.SetName("DFSGraph:uptree");
  DoDFS(b0);
  } 

    DFSGraph(GraphContainer &GC,DFSGraph &GG) :
        GraphAccess(GC), G(GG), Gvin(GG.Set(tbrin()),PROP_VIN),
        nvin(Set(tbrin()),PROP_VIN), _ib(Set(tedge()),PROP_INITIALB), uptree(G.nv(),G.ne())
        {setsize(GG.nv(),GG.ne());
        nvin.SetName("DFSGraph:2:nvin");
        _ib.SetName("DFSGraph:2:_ib");
        nvin = GG.nvin;
        for (tbrin b=1; b<=ne(); b++) _ib[b]=b;
        }
    DFSGraph(DFSGraph &DG) : 
	GraphAccess(DG), G(DG.G), Gvin(DG.G.Set(tbrin()),PROP_VIN),
      nvin(Set(tbrin()) ,PROP_VIN), _ib(Set(tedge()),PROP_INITIALB),uptree(DG.uptree)
        {}
    ~DFSGraph() {}
    int Kuratowski();
    int CotreeCritical();
    tbrin treein(tvertex v) {return v()-1;}
    tvertex treetarget(tedge e) {return e()+1;}
    tvertex father(tvertex v) {return nvin[v()-1];}
    tbrin ib(tbrin b) 
        {if (b()>=0) return _ib[b](); else return -_ib[-b]();}
    tedge ie(tedge e)
        {return _ib[e].GetEdge();}
    tvertex iv(tvertex v)
        {if (v==0) return 0;
        else if (v==1) return Gvin[_ib[1]];
        else return Gvin[-_ib[v()-1]];
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
        return;
        }
    tedge NewCotreeEdge(tvertex v1, tvertex v2, tbrin ibe=0)
	{ setsize(tedge(),ne()+1);
	nvin[ne()]=v1;
	nvin[-ne()]=v2;
	_ib[ne()]=ibe;
	return ne();
	}
    void MarkFundTree();
    void DeleteCotreeEdge(tedge e);
    void ComputeDegrees();
    void ShrinkSubdivision();
    int MarkKuratowski();
    void Shrink();
    void ShrinkCotree();
    int DoDFS(tbrin b0=1);
    int bicon();
    int TestPlanar();
    int Planarity();
};

#endif
