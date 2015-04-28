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


#ifndef TPROPTGF_H
#define TPROPTGF_H
#include <TAXI/Tprop.h>
#include <TAXI/Tgf.h>
#ifdef PRIVATE
#define MAGIC_PROP 0x1000
#define PSetHeaderTag(SetNum) (short) (MAGIC_PROP | (SetNum<<8))
#define PSetTag(SetNum,PNum) (short) (MAGIC_PROP | 0x400 | (SetNum<<8) | PNum)
#define PSet1Tag(SetNum,PNum) (short) (MAGIC_PROP | 0x800 | (SetNum<<8) | PNum)
struct pset_header
{
    int start;
    int finish;
};
#endif
void WriteTGF(PSet &X,Tgf &file,int PNum);
void ReadTGF(PSet &X, Tgf &file, int PNum);
void WriteTGF(PSet1 &X,Tgf &file,int PNum);
void ReadTGF(PSet1 &X, Tgf &file, int PNum);
#endif
