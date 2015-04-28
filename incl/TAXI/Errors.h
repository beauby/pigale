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

#ifndef  ERRORS_H_INCLUDED
#define  ERRORS_H_INCLUDED 
 
#define A_ERRORS_PIGALE                  (-1000) 
#define A_ERRORS_BAD_INPUT               (A_ERRORS_PIGALE-1)  // bad_input
#define A_ERRORS_PLANARITY               (A_ERRORS_PIGALE-2)  // Planarity
#define A_ERRORS_GDFSRENUM               (A_ERRORS_PIGALE-3)  // GDFSRenum
#define A_ERRORS_TUTTECIRCLE             (A_ERRORS_PIGALE-4)  // TutteCircle
#define A_ERRORS_BARYCENTER              (A_ERRORS_PIGALE-5)  // ComputeBary
#define A_ERRORS_BIPOLAR_PLAN            (A_ERRORS_PIGALE-6)  // PseudoBipolarPlan
#define A_ERRORS_BIPOLAR_PSEUDO_PLAN     (A_ERRORS_PIGALE-7)  // BipolarPlan
#define A_ERRORS_VTRIANGULATE            (A_ERRORS_PIGALE-8)  // Vertextriangulate
#define A_ERRORS_SUBDIVTRICON            (A_ERRORS_PIGALE-9)  // SubdivTriconencted
#define A_ERRORS_DUAL                    (A_ERRORS_PIGALE-10) // Dual


#endif
