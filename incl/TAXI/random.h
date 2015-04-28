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

#ifndef RANDOM_H
#define RANDOM_H 

long & randomSetSeed();            // set the seed of the random generator 
void randomInitSeed();             // called once, when program  start
void randomStart();                // called when needed randoms number
void randomEnd();                  // save a new seed
long randomGet(long range);        // returns an integer >= 1 && <= range  
long randomGet();
void randomShuffle(svector<int> &tab); // randomly permuts the elements of tab
unsigned long randomMax();
#endif
