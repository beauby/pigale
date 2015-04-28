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

#ifndef _DFSLOW_H_
#define _DFSLOW_H_

#include <TAXI/Tbase.h>
#include <TAXI/color.h>
#include <TAXI/DFSGraph.h>
#define PRIVATE
#include <TAXI/lralgo.h>
#undef PRIVATE
#include <TAXI/Tdebug.h>

struct DFSLow : public DFSGraph
{
    Prop<tedge> elow;
    Prop<tvertex> low;
    Prop<int> status;
    
    DFSLow(DFSGraph &DG) : DFSGraph(DG),
        elow(Set(tvertex()),PROP_ELOW), 
        low(Set(tvertex()),PROP_LOW), status(Set(tvertex()),PROP_TSTATUS)
        {elow.SetName("DFSLow:elow");
        low.SetName("DFSLow:low");
        status.SetName("DFSLow:status");
        bicon();
        }
    DFSLow(DFSLow &DL) : DFSGraph(DL),
        elow(Set(tvertex()),PROP_ELOW), 
        low(Set(tvertex()),PROP_LOW), status(Set(tvertex()),PROP_TSTATUS)
        {}    
    ~DFSLow() {}
    int TestPlanar();
    void LralgoSort(_LrSort &LrSort);
    int Lralgo( _LrSort &LrSort, _FastHist &Hist);
    int Planarity();
    int Lralgo(const _LrSort &LrSort, _Hist &Hist);
    
};
#endif
