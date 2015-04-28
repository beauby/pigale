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

/*!
\file Handler.h
\brief Handlers used to call the algorithms.

PigaleThread calls handler to select which algorithm to execute. <br>
Each algorithm to call is identified by a unique integer action defined in Action_def.h.<br>
The return value is used to known which action should be performed after.<br>
The possible return values are:
<ul>
 <li>-1 if error   </li>
 <li>O nothing to do   </li>
 <li>1 update the editor   </li>
 <li>2 update the editor and the graph properties   </li>
 <li>20 update the editor, without any recomputing   </li>
 <li>3 there is a drawing to do (pigalePaint)   </li>
 <li>4 make a R<sup><small>d</small></sup> drawing  </li>
 <li>5 look for symetries   </li>
</ul>
*/

#ifndef HANDLER_H
#define HANDLER_H

#include <Pigale.h> 

bool & SchnyderRect();
bool & SchnyderLongestFace();
bool & SchnyderColor();
int EmbedHandler(GraphContainer &GC,int action,int &drawing); 
int OrientHandler(GraphContainer &GC,int action); 
int AlgoHandler(GraphContainer &GC,int action); 
int DualHandler(GraphContainer &GC,int action); 
int RemoveHandler(GraphContainer &GC,int action); 
int AugmentHandler(GraphContainer &GC,int action); 
int AlgoHandler(GraphContainer &GC,int action,int nn); 
int GenerateHandler(GraphContainer &GC,int action,int n1_gen,int n2_gen,int m_gen);
// in Generate.cpp
bool & EraseMultipleEdges();
#endif
