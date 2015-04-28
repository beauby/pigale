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
/*! \file propdef.h
 * Definition of the property number for sets General,V,B,E.
 */
#ifndef PROPDEF_H
#define PROPDEF_H
/*! \file propdef.h
 * Property numbers for property sets (General,V,B,E)
 */
/** @name Property numbers for all sets
  */
//@{
#define PROP_DRAWING          (150)               //!< Start of Drawing Properties
#define PROP_DRAW_COORD       (PROP_DRAWING)      //!< \c Tpoint Main point
#define PROP_DRAW_POINT_1     (PROP_DRAWING+1)    //!< \c Tpoint
#define PROP_DRAW_POINT_2     (PROP_DRAWING+2)    //!< \c Tpoint
#define PROP_DRAW_POINT_3     (PROP_DRAWING+3)    //!< \c Tpoint
#define PROP_DRAW_POINT_4     (PROP_DRAWING+4)    //!< \c Tpoint
#define PROP_DRAW_POINT_5     (PROP_DRAWING+5)    //!< \c Tpoint
#define PROP_DRAW_POINT_6     (PROP_DRAWING+6)    //!< \c Tpoint
#define PROP_DRAW_DBLE_1      (PROP_DRAWING+11)   //!< \c double
#define PROP_DRAW_DBLE_2      (PROP_DRAWING+12)   //!< \c double
#define PROP_DRAW_DBLE_3      (PROP_DRAWING+13)   //!< \c double
#define PROP_DRAW_DBLE_4      (PROP_DRAWING+14)   //!< \c double
#define PROP_DRAW_DBLE_5      (PROP_DRAWING+15)   //!< \c double
#define PROP_DRAW_DBLE_6      (PROP_DRAWING+16)   //!< \c double
#define PROP_DRAW_DBLE_MIN    (PROP_DRAWING+17)   //!< \c double
#define PROP_DRAW_DBLE_MAX    (PROP_DRAWING+18)   //!< \c double
#define PROP_DRAW_INT_1       (PROP_DRAWING+21)   //!< \c int
#define PROP_DRAW_INT_2       (PROP_DRAWING+22)   //!< \c int
#define PROP_DRAW_INT_3       (PROP_DRAWING+23)   //!< \c int
#define PROP_DRAW_INT_4       (PROP_DRAWING+24)   //!< \c int
#define PROP_DRAW_INT_5       (PROP_DRAWING+25)   //!< \c int
#define PROP_DRAW_INT_6       (PROP_DRAWING+26)   //!< \c int
#define PROP_NOTSAVED          (128)               //!< Start of not SavedProperties
#define PROP_RESERVED         (200)               //!< \c anything Reserved for applications 
#define PROP_TMP              (255)               //!< \c anything Temporary property
//@}
/** @name  Property numbers for General set.
  */
//@{
#define PROP_TITRE            (0)    //!< \c tstring Graph Title
#define PROP_CONNECTED        (1)    //!< \c int Graph is connected
#define PROP_BICONNECTED      (2)    //!< \c int Graph is biconnected
#define PROP_TRICONNECTED     (3)    //!< \c int Graph is triconnected
#define PROP_FOURCONNECTED    (4)    //!< \c int Graph is 4-connected
#define PROP_PLANAR           (5)    //!< \c int Graph is planar
#define PROP_PLANARMAP        (6)    //!< \c int Graph is planar embedded
#define PROP_N                (7)    //!< \c int Number of vertices (backup)
#define PROP_M                (8)    //!< \c int Number of edges (backup)
#define PROP_EXTBRIN          (9)    //!< \c tbrin External brin (defines outer face)
#define PROP_SIMPLE           (10)   //!< \c int Graph is simple
#define PROP_BIPARTITE        (12)   //!< \c int Graph is bipartite
#define PROP_MAPTYPE          (13)   //!< \c int Embedding type
#define PROP_MAPTYPE_UNKNOWN         (0) //!< Unknown Origin
#define PROP_MAPTYPE_ARBITRARY       (1)          //!< Arbitrary Map
#define PROP_MAPTYPE_GEOMETRIC       (2)          //!< Geometric Map
#define PROP_MAPTYPE_LRALGO          (3)          //!< Map computed by LR-Algo
#define PROP_SYM              (14)   //!< \c int Has a symetric labeling
#define PROP_HYPERGRAPH       (15)   //!< \c int Graph is an hypergraph
#define PROP_NV               (16)   //!< \c int #hyper-vertices/red vertices
#define PROP_NE               (17)   //!< \c int #hyper-edgees/blue vertices
#define PROP_NCV              (18)   //!< \c int # classes for vertices
#define PROP_NCE              (19)   //!< \c int # classes for edges
#define PROP_NLOOPS           (30)   //!< \c int no loops
#define PROP_MN               (31)   //!< \c int #meta vertices
#define PROP_MM               (32)   //!< \c int #meta edges
#define PROP_VSLABEL          (33)   //!< \c svector<tstring *> numerical label to string label for vertices
#define PROP_ESLABEL          (34)   //!< \c svector<tstring *> numerical label to string label for edges
#define PROP_POINT_MIN       (PROP_DRAWING+1)    //!< \c Tpoint
#define PROP_POINT_MAX       (PROP_DRAWING+2)    //!< \c Tpoint
#define PROP_RNEMBED         (PROP_DRAWING+3)    //!< \c RnEmbeddingPtr
//@}
/** @name Property numbers for V,B,E.
  */
//@{ 
#define PROP_LABEL            (0)    //!< \c int  Label
#define PROP_COLOR            (1)    //!< \c short Color
#define PROP_MARK             (2)    //!< \c bool  Various marks
#define PROP_TYPE             (7)    //!< \c int   Associated type
#define PROP_FLAGS            (8)    //!< \c long  Various Flags
#define PROP_ORDER            (9)    //!< \c int   Priority order
#define PROP_PMETA            (10)   //!< \c tbrin First meta brin
#define PROP_BIRTH            (11)   //!< \c long  Start of validity
#define PROP_DEATH            (12)   //!< \c long  End of validity
#define PROP_VALUE            (13)   //!< \c double A value
#define PROP_SLABEL           (15)   //!< \c int   String label index

//@}
/** @name Property numbers for V.
  */
//@{
#define PROP_INITIALV         (3)    //!< \c tvertex  Initial Numbering
#define PROP_NEW              (4)    //!< \c tvertex  New numbering
#define PROP_CLASSV           (5)    //!< \c tvertex  Class of vertex
#define PROP_REPRESENTATIVEV  (6)    //!< \c tvertex  Representative of vertex
#define PROP_COORD            (16)   //!< \c Tpoint   Coord for editor
#define PROP_PBRIN            (17)   //!< \c tbrin    First incident brin
#define PROP_STLINK           (18)   //!< \c tvertex  link of st-ordering
#define PROP_LOW              (19)   //!< \c tvertex  DFS low
#define PROP_ELOW             (20)   //!< \c tedge    DFS edge-low
#define PROP_TSTATUS          (21)   //!< \c char     Status of vertex or incoming tree
#define PROP_TSTATUS_ISTHMUS          0           //!< articulation
#define PROP_TSTATUS_LEAF             1           //!< articulation pair
#define PROP_TSTATUS_THIN             2           //!< thin incoming tree edge 
#define PROP_TSTATUS_THICK            3           //!< thick incoming tree edge
#define PROP_DUALORDER        (22)   //!< \c int  DFS Dual Order (Kura)
#define PROP_DEGREE           (25)   //!< \c int  vertex degree
#define PROP_SYMLABEL         (26)   //!< \c int    label in the symetry
#define PROP_REDUCED          (28)   //!< \c tvertex class in reduced graph
#define PROP_HYPEREDGE        (29)   //!< \c bool    vertex is an hyperedge
#define PROP_NLOOPS           (30)   //!< \c int     # of attached loops
#define PROP_COORD3          (PROP_DRAWING+31)   //!< \c Tpoint3 Coord for Embed3d
#define PROP_EIGEN           (PROP_NOTSAVED+32)   //!< \c double eigenvalues for Embed3d
#define PROP_CANVAS_ITEM     (PROP_NOTSAVED+1)     //!< \c void * NodeItem
#define PROP_CANVAS_COORD    (PROP_NOTSAVED+2)     //!< \c Tpoint   Copy Coord for editor

//@}
/** @name Property numbers for E.
  */
//@{
#define PROP_INITIALE         (3)    //!< \c tedge   Initial Numbering
#define PROP_CLASSE           (5)    //!< \c tedge   Class of edge
#define PROP_REPRESENTATIVEE  (6)    //!< \c tedge   Representative of edge
#define PROP_COLOR2           (14)   //!< \c short Color of the second brin
#define PROP_WIDTH            (16)   //!< \c int     geometric width (editor)
#define PROP_ORIENTED         (17)   //!< \c bool    edge is oriented
#define PROP_REORIENTED       (18)   //!< \c bool    orientation should be reversed
#define PROP_ISTREE           (19)   //!< \c bool    edge belongs to the tree
#define PROP_MULTIPLICITY     (20)   //!< \c int     edge multiplicity
#define PROP_CANVAS_ITEM     (PROP_NOTSAVED+1)     //!< \c void * EdgeItem
#define PROP_ISTREE_LR           (PROP_NOTSAVED+2)   //!< \c bool    edge belongs to the tree of the LR-algorithm
//@}
/** @name Property numbers for B.
  */
//@{
#define PROP_INITIALB         (3)    //!< \c tbrin   Initial Numbering
#define PROP_CLASSB           (5)    //!< \c tbrin   Class of brin
#define PROP_VIN              (16)   //!< \c tvertex vertex incidence
#define PROP_CIR              (18)   //!< \c tbrin   next brin in circular order
#define PROP_ACIR             (19)   //!< \c tbrin   previous brin in circular order
#endif
//@}

const char *PropName(int s,int i);
const char *PropDesc(int s,int i);
