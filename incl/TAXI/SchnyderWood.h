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

#ifndef __SCHWOOD_H__
#define __SCHWOOD_H__

#include <TAXI/graphs.h>
#include <TAXI/IntList.h>
#include <TAXI/Tdebug.h>
#include "TAXI/color.h"

// This class manipulates Schnyder wood of 3-connected planar graphs, as defined in the article of Stefan Felsner: "Geodesic Embeddings and Planar Graphs",Order 20 (2003), 135-150. 
// For each internal vertex, 3 outgoing brins are colored (Red/Blue/Green).
// If an edge is single, only one of its brin is colored (Red/Blue/Green). The other is colored black.
// When applying cw_merge or ccw_merges, some edges may no longer belong to the Schnyder wood. In this case, the edge becomes colored black.

class SchnyderWood {
    TopologicalGraph G;
    tbrin FirstBrin; //brin from v_1 to v_2

    // Roots of the trees (blue, green, red)
    tvertex v_1,v_2,v_n;

    // Parent brin in the blue tree
    svector<tbrin> ParentB; 

    // Parent brin in the green tree
    svector<tbrin> ParentG; 

    // Parent brin in the red tree
    svector<tbrin> ParentR; 

    // Color of a brin
    svector<short> brin_color; 

    // Returns true, if the tbrin b is the lowest of the face
    bool isFirstBrinOfFace(tbrin b) const;

    //Returns true if the face at the right of the brin is unpointed
    bool is_unpointed_face(tbrin b) const;

    public:

    // b is the brin indicating the outer face.
    // Typically, b is the brin defined by the vertex v_n and the edge {v_1,v_n}.
    // This brin can be found by the function: GeometricGraph::FindExteriorface().
    // Construct THE minimal Schnyder wood of G0 (using the algorithm
    // presented in E. Fusy, D. Poulalhon & G. Schaeffer, "Coding, counting and sampling 3-connected planar graphs", SODA'05).
    // In such SW, all edges are colored.
    SchnyderWood(Graph &G0, tbrin b);

    ~SchnyderWood() {}


    //Get the root of the tree colored c
    tvertex GetRoot(short c) const;
    
    // Indicates if the edge is single or double
    bool IsSimple (tedge e) const;

    // Indicates if the edge belongs to the Schnyder wood or not.
    // When an edge is merged, it becomes black.
    bool IsBlack (tedge e) const;

    // Indicates if the vertex is the root of one of the 3 trees.
    bool IsRoot (tvertex v) const;


    //Returns the outgoing brin colored c of the vertex v.
    // If The vertex v is the root of the tree c, the function return 0.
    tbrin GetParentBr(tvertex v, short c) const;

    //Returns the outgoing edge colored c of the vertex v.
    // If The vertex v is the root of the tree c, the function return 0.
    tedge GetParentE (tvertex v, short c) const;
    
    //Returns the parent of the vertex v in the tree colored c.
    // If The vertex v is the root of the tree c, the function return 0.
    tvertex GetParentV (tvertex v, short c) const;

    //Returns the color of the brin b and the opposite brin of b.
    void GetEdgeColors (tbrin b, short &c1, short &c2) const;
    

    //Returns the color of the brin b
    short GetBrinColor (tbrin b) const;
    

    //Removes the cw-elbow by a ccw-elbow
    //Returns true if the operation was possible
    bool remove_cw_elbow (tvertex v, short c);

    //Removes the ccw-elbow with  a cw-elbow
    //Returns true if the operation was possible
    bool remove_ccw_elbow (tvertex v, short c);

    //Checks if the outgoing edge colored c is simple and if the counter-clockwise next edge
    // is double colored c', c"
    bool is_cw_elbow(tvertex v, short c) const;

    //Checks if the outgoing edge colored c is simple and if the clockwise next edge
    // is double colored c', c"
    bool is_ccw_elbow(tvertex v, short c) const;


    //Check if the face containing the red parent of v, 
    // contains a counterclockwise edge of each color
    bool is_ccw_smooth(tvertex v) const;

    // Check if the face containing the red parent of v, 
    // contains a clockwise edge of each color.
    bool is_cw_smooth(tvertex v) const;

    // Check if v is a blue-red corner of a triangular cw_face
    bool is_cw_face(tvertex v) const;
    
    // Replaces, if possible,  a the triangular cw_face such aht c is a blue-red corner by a ccw_face
    //Returns true if the face was a cw_face
    bool reverse_cw_face(tvertex v);

    // Check if v is a red-blue corner of a triangular ccw_face
    bool is_ccw_face(tvertex v) const;

    // Replaces a the triangular ccw_face such aht c is a blue-red corner by a cw_face
    //Returns true if the face was a ccw_face
    bool reverse_ccw_face(tvertex v);

    //Makes, if possible, a clockwise merge, of the outgoing edge of v colored c.
    //Returns true if the operation was possible
    bool cw_merge (tvertex v, short c);

    //Makes, if possible, a counter-clockwise merge, of the outgoing edge of v colored c.
    //Returns true if the operation was possible
    bool ccw_merge (tvertex v, short c);

    // Returns the color before and after c in this list (Red/Blue/Green)
    void CyclicColors(short c, short &c_left, short &c_right) const;

    // Returns the clockwise first child in the tree c
    // If v is a leaf, then the function returns 0;
    tvertex GetFirstChild( tvertex v, short c) const;

    // Returns the clockwise next child afert current_child in the tree c.
    // If current_child is the last child of v, then the function returns 0;
    tvertex GetNextChild (tvertex current_child, short c) const;

    // Returns the clockwise last child in the tree c
    // If v is a leaf, then the function returns 0;
    tvertex GetLastChild( tvertex v, short c) const;

    // Returns the clockwise next child afert current_child in the tree c.
    // If current_child is the first child of v, then the function returns 0;
    tvertex GetPreviousChild (tvertex current_child, short c) const;


    // Counts the number of cw-triangles of simple edges together with the cw-elbows.
    int CountDeltaSharpCW () const;

    // Counts the number of ccw-triangles of simple edges together with the ccw-elbows.
    int CountDeltaSharpCCW () const;

    // Counts the number of faces with a clockwise edge in each of the tree colors.
    int CountDeltaSmoothCW () const;

    // counts the number of faces with a counterclockwise edge in each of the tree colors.
    int CountDeltaSmoothCCW () const;

    // counts the number of unpointed faces
    int CountUnpointedFaces() const;

};


#endif
