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

#ifndef EMBED_H
#define EMBED_H
#include <TAXI/graph.h>
#include <TAXI/graphs.h>
#include <TAXI/DFSLow.h>

// structure defining the Side of edges
struct _Side {
  svector<int> up;		// Side of the upper edge end
  svector<int> dwn;		// Side of the lower edge end

  _Side(int m) : up(0,m), dwn(0,m)
      {}
  ~_Side() {}
};

#define GAUCHE  0
#define DROITE  1
#define AUTRE   2

class Embed {
  int n;
  int m;
  const svector<int> &status;
  const _LrSort &LrSort;
  _Hist &Hist;
  Prop<tbrin> cir;
  Prop<tbrin> acir;
  Prop<tvertex> vin;
  Prop1<tbrin> extbrin;
  _Side Side;

public:
  Embed(Graph &G, const _Bicon &rBicon, const _LrSort &rLrSort, _Hist &rHist) :
      n(G.nv()), m(G.ne()), status(rBicon.status),LrSort(rLrSort),
      Hist(rHist),
      cir(G.PB(),PROP_CIR), acir(G.PB(),PROP_ACIR), vin(G.PB(),PROP_VIN),
      extbrin(G.Set(),PROP_EXTBRIN),Side(m)
      {}
  Embed(DFSLow &G, const _LrSort &rLrSort, _Hist &rHist) :
      n(G.nv()), m(G.ne()), status(G.status),LrSort(rLrSort),
      Hist(rHist),
      cir(G.PB(),PROP_CIR), acir(G.PB(),PROP_ACIR), vin(G.PB(),PROP_VIN),
      extbrin(G.Set(),PROP_EXTBRIN),Side(m)
    {}
    ~Embed() {}

  void operator()()
      {LowerSideUpperSide();
      //	PrintLowerSideUpperSide();
      FillUpperHalfEdges();
      FillLowerHalfEdges();
      }
 protected:
  int LowerSideUpperSide();
  void PrintLowerSideUpperSide();
  void FillUpperHalfEdges();
  int  FillLowerHalfEdges();
  void fuse(tvertex vi,tbrin topri,tbrin toprif,tbrin topli,tbrin toplif,	tbrin botli,tbrin botlif,tbrin botri,tbrin botrif);
  void bun_in(tbrin ee,tbrin ebu,svector<tbrin>& left)
      {tbrin je;
      if (acir[cir[ee]] == ee) return;           // d‚j… fait
      if (Side.dwn[ee] & DROITE)                     // brin a droite
          circu_in(ee,ebu,cir[ebu]);
      else                                           // brin a gauche
          {je = left[ebu];
          circu_in(ee,acir[je],je);
          left[ebu] = ee;
          }
      }
  void circu_in(tbrin ee,tbrin previous,tbrin next)
      {cir[previous] = ee; cir[ee] = next;
      acir[next] = ee; 	acir[ee] = previous;
      }
  void ins_bot(tbrin ee,tbrin &bot,tbrin &top)
      {if(bot!=0)
          {cir[bot] = ee; bot = ee;}
      else
          {bot = top = ee;}
      }
  void ins_top(tbrin ee,tbrin &bot,tbrin &top)
      {if(top!=0)
          {cir[ee] = top;top = ee;}
      else
          {bot = top = ee;}
      }
};
#endif







