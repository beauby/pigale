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

#ifndef __SCHPACK_H__
#define __SCHPACK_H__

#include <TAXI/graphs.h>
#include <TAXI/IntList.h>
#include <TAXI/Tdebug.h>

class SchnyderPacking {
    TopologicalGraph G;
    tbrin FirstBrin;
    tvertex v_1,v_2,v_n;
    svector<int> MarkedV, MarkedE, n_cones;
    IntList packable;
    int count;
    int mode;   // 1: rightmost packing 2: leftmost packing 3: manual

    void MarkEdge(tbrin b) ;
    void pack(tvertex v, tbrin & left_brin, tbrin & right_brin);

    public:

    // b is the brin indicating the outer face.
    // Typically, b is the brin defined by the vertex v_n and the edge {v_1,v_n}.
    // This brin can be found by the function: GeometricGraph::FindExteriorface().
    SchnyderPacking(Graph &G0, tbrin b) : G(G0), FirstBrin(b),
        v_1(G.vin[-b]), v_2(G.vin[-G.cir[-b]]), v_n(G.vin[b]),
        MarkedV(1,G.nv(),0), MarkedE(1,G.ne(),0), n_cones(1,G.nv(),0),
        count(0), mode(1)
        {
        MarkedV.SetName("MarkedV");
        MarkedE.SetName("MarkedE");
        n_cones.SetName("n_cones");

        MarkedE[G.cir[-b].GetEdge()] = 1;
        packable.push(v_2());
        packable.push(v_1());
        }
    ~SchnyderPacking() {}
    tvertex FindVertex(tbrin &left, tbrin &right);
    tvertex FindVertex();
};


void SchnyderDecomp(TopologicalGraph &G, tbrin brin,svector<short> &ecolor);

#endif
