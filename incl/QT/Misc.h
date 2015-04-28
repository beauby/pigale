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

#ifndef MISC_H
#define MISC_H

#include <QString> 

// in Misc.cpp 
QString getVertexLabel(GraphContainer &GC,tvertex v);
void DefinepigaleWindow(pigaleWindow *father);
pigaleWindow* GetpigaleWindow();
void DrawGraph(void);
void DrawGraph(Graph &G);
//int GraphInformation();
QString universalFileName(QString const & fileName);
QString  getPigaleErrorString();
void Normalise();
void Normalise(GeometricGraph &G); 
void Normalise(TopologicalGraph &G);
int & pauseDelay();
QColor OppCol(QColor & col);
QColor Desaturate(QColor & col);

#endif
