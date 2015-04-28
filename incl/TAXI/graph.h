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

#ifndef TGRAPH_H
#define TGRAPH_H

#include <TAXI/elmt.h>
#include <TAXI/Tprop.h>
#include <TAXI/propdef.h>
#include <TAXI/Tdebug.h>


class GraphContainer
{
 protected :
    PSet1 General;
    PSet V;
    PSet E;
    PSet B;
    int n;
    int m;

public:
    GraphContainer() : General(),V(0,0),E(0,0),B(0,0), n(0),m(0) {}
    GraphContainer(const GraphContainer &G) : General(G.General)
        ,V(G.V),E(G.E),B(G.B), n(G.n),m(G.m)    {}

    ~GraphContainer() {}

    GraphContainer & operator=(const GraphContainer &G)
        {if(this == &G)return *this; 
        General = G.General; V=G.V; E=G.E; B=G.B; m=G.m; n=G.n;
        return *this;
        }
    void Tswap(GraphContainer &G)
        { int tmp = n; n=G.n; G.n=tmp;
        tmp = m; m=G.m; G.m=tmp;
        General.Tswap(G.General);
        V.Tswap(G.V);
        B.Tswap(G.B);
        E.Tswap(G.E);
        }
    int nv() const {return n;}
    int ne() const {return m;}
	 int nbre(const tvertex &) const {return n;}
	 int nbre(const tedge &) const {return m;}
	 PSet &Set(const tvertex &) {return V;}
	 PSet &Set(const tedge &) {return E;}
	 PSet &Set(const tbrin &) {return B;}
	 PSet1 &Set() {return General;}
	 PSet &PV() {return V;}
	 PSet &PB() {return B;}
	 PSet &PE() {return E;}
	 PSet1 &PG() {return General;}
	 void setsize(const tvertex &,int i) {n=i; V.resize(0,i);}
	 void setsize(const tedge &,int i) {m=i; E.resize(0,i); B.resize(-i,i);}
	 void setsize(int nn,int mm)
		{setsize(tvertex(),nn); setsize(tedge(),mm);}
	 void incsize(const tvertex &) {n++; V.resize(0,n);}
	 void decsize(const tvertex &) {n--; V.resize(0,n);}
	 void incsize(const tedge &) {m++; E.resize(0,m); B.resize(-m,m);}
	 void decsize(const tedge &) {m--; E.resize(0,m); B.resize(-m,m);}
	 void clear()
		  { n=m=0;General.clear(); V.clear(); E.clear(); B.clear();}
     void reset()
         { General.reset(); V.reset(); E.reset(); B.reset(); setsize(0,0);}
     void ClearKeep() {General.KeepClear(); V.KeepClear(); E.KeepClear(); B.KeepClear();}
};
class GraphAccess
{
protected :
	 GraphContainer &rG;
 public:
    void keepr()
        {
        }
    void keeponly()
        {
        ClearKeep();
        }
public:
	 GraphAccess(GraphContainer &G)  : rG(G){}
	 GraphAccess(GraphAccess &G) : rG(G.rG){}
	 ~GraphAccess() {}

    GraphAccess & operator=(const GraphContainer & G) {rG=G; return *this;}
    int nv() const {return rG.nv();}
    int ne() const {return rG.ne();}
	 int nbre(const tvertex &) const {return nv();}
	 int nbre(const tedge &) const {return ne();}
	 PSet &Set(const tvertex &) {return rG.PV();}
	 PSet &Set(const tedge &) {return rG.PE();}
	 PSet &Set(const tbrin &) {return rG.PB();}
	 PSet1 &Set() {return rG.PG();}
	 PSet &PV() {return rG.PV();}
	 PSet &PB() {return rG.PB();}
	 PSet &PE() {return rG.PE();}
	 PSet1 &PG() {return rG.PG();}
	 void setsize(const tvertex &v,int i) {rG.setsize(v,i);}
	 void setsize(const tedge &e,int i) {rG.setsize(e,i);}
	 void setsize(int nn,int mm) {rG.setsize(nn,mm);}
	 void incsize(const tvertex &v) {rG.incsize(v);}
	 void decsize(const tvertex &v) {rG.decsize(v);}
	 void incsize(const tedge &e) {rG.incsize(e);}
	 void decsize(const tedge &e) {rG.decsize(e);}
	 // void clear() {rG.clear();}
     void reset() {rG.reset();}
     void StrictReset() {keeponly(); reset();}
     void ClearKeep() {rG.ClearKeep();}
	 GraphContainer &me() {return rG;}
	 const GraphContainer &me() const {return rG;}
	 GraphContainer &Container() {return rG;}
	 const GraphContainer &Container() const {return rG;}
     void Tswap(GraphContainer &G) { rG.Tswap(G);}
 
};
#define ForAllEdges(e,G)     for(e=1;e<=G.ne();e++)
#define ForAllVertices(v,G)  for(v=1;v<=G.nv();v++)
#define ForAllEdgesOfG(e)       for(e=1;e<=ne();e++)
#define ForAllVerticesOfG(v)    for(v=1;v<=nv();v++)
#endif
