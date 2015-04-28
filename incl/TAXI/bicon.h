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

#ifndef BICON_H
#define BICON_H
#include <TAXI/graph.h>

struct _Bicon {
    svector<int> status;
    _Bicon(int n)  : status(0,n) { status.clear();status.SetName("Bicon.status");}
    ~_Bicon(){}
};
#endif
