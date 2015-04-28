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

#ifndef __KANTSHEL_H__
#define __KANTSHEL_H__

#include <TAXI/graphs.h>
#include <TAXI/IntList.h>
#include <TAXI/Tdebug.h>

class KantShelling {
 protected:
    TopologicalGraph G;

 private:

    int nf;                          // #faces.
    tbrin FirstBrin;                 // = (v_1,v_2).
    tvertex v_1,v_2,v_n;

    IntList Candidates;              // stack of faces and vertices to shelled.
                                     // positive value for faces,
                                     // negative value for vertices.
    svector<int> Brin2Face;          // brin -> face
    svector<int> Face2Brin;          // face -> a brin
    svector<int> IsOuterV, IsOuterE; // 1 iff incident to F_out.                                    

    svector<int> MarkedV, MarkedE, MarkedF, visited, sepf, outv, oute;
    svector<int> BelongToLastFace;
    
    int error;

    void _visited(int v, int i);
    void _sepf(int v, int i);
    void _outv(int f, int i);
    void _oute(int f, int i);
    void UpdateSepf1(IntList &NewSepFaces);
    void UpdateSepf2(IntList &NewOuterVertices);
    void SetAdjFaces() ;

    public:

    // b is the brin defined by the vertex v_1 and the edge {v_1,v_2}.
    KantShelling(Graph &G0, tbrin bref) : G(G0), nf(G.NumFaces()), FirstBrin(bref),
        v_1(G.vin[bref]), v_2(G.vin[-bref]), v_n(G.vin[-G.acir[bref]]),
        Brin2Face(-G.ne(),G.ne()),
        Face2Brin(1,nf),
        IsOuterV(1,G.nv(),0), IsOuterE(1,G.ne(),0),
        MarkedV(1,G.nv(),0), MarkedE(1,G.ne(),0), MarkedF(1,nf,0),
        visited(1,G.nv(),0), sepf(1,G.nv(),0), outv(1,nf,0), oute(1,nf,0)
        ,BelongToLastFace(1,G.nv())
        ,error(0)
        {
        if (G.ComputeGenus()) {error=1; return;}
        G.CheckTriconnected();
        if(!G.Set().exist(PROP_TRICONNECTED)){error=1; return;}
        BelongToLastFace.clear();
        MarkedV.SetName("MarkedV");
        MarkedE.SetName("MarkedE");
        MarkedF.SetName("MarkedF");
        BelongToLastFace.SetName("BelongTo");
        
        SetAdjFaces(); // set Brin2Face, Face2Brin, IsOuterV, IsOuterE.

        tbrin b, b0;

        // initialize outv.                            
        int iv;
        for (iv = 1;iv <= G.nv();iv++)
            if(IsOuterV[iv])   // if v is incident to the outer face...
                {b0 = b =G.FirstBrin(iv);
                do
                    outv[Brin2Face[b]]++;
                while((b = G.cir[b]) != b0);
                }                                            
            
        // initialize oute.                            
        b = FirstBrin;
        b = G.cir[-b]; 
        // Skip  edge {v_1,v_2}. Otherwise, the last face would not be shelled.
        do 
            oute[Brin2Face[-b]]++; 
        while ((b = G.cir[-b])  != FirstBrin);
        _visited(v_n(),1);
        }

    // return : 1 if a vertex is shelled, 
    //          k if a chain of length k is shelled,
    //          0 if no face nor vertex to pack.
    int FindNext(tbrin &left, tbrin &right);

    int Error() const { return error; }

};


class LMCShelling : public KantShelling {
    svector<tbrin> LeftBrin;       
    svector<tbrin> RightBrin;      
    svector<int> NbBrin;          

    svector<tbrin> NewLeftBrin;   
    svector<tbrin> NewRightBrin;  
    svector<int> NewNbBrin;       

    int current;
    public:

    // b is the brin defined by the vertex v_1 and the edge {v_1,v_2}.
    LMCShelling(Graph &G0, tbrin bref);

    // return : 1 if a vertex is shelled, 
    //          k if a chain of length k is shelled,
    //          0 if no face nor vertex to pack.
    int FindNext(tbrin &left, tbrin &right);

};

#endif
